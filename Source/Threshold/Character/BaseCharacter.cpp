// Copyright (c) 2020 Spencer Melnick

#include "BaseCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/Movement/THCharacterMovement.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Abilities/THGameplayAbility.h"
#include "Threshold/Global/Subsystems/CombatantSubsystem.h"




// Component name constants

FName ABaseCharacter::AbilitySystemComponentName(TEXT("AbilitySystemComponent"));
FName ABaseCharacter::DodgeRootMotionName(TEXT("DodgeRootMotionInstance"));




// Default constructor

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(ACharacter::CharacterMovementComponentName,
		UTHCharacterMovement::StaticClass()))
{
	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	// Create our default ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UTHAbilitySystemComponent>(AbilitySystemComponentName);
	AbilitySystemComponent->SetIsReplicated(true);

	// Don't replicate gameplay effects - suggested for AI controlled characters
	AbilitySystemComponent->ReplicationMode = EGameplayEffectReplicationMode::Minimal;
	
	// Drive our rotation using the movement component instead of directly reading the control rotation
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}




// Engine overrides

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Try to set up our starting abilities
	GrantStartingAbilities();

	// Register this character as a combatant
	UCombatantSubsystem* CombatantSubsystem = GetWorld()->GetSubsystem<UCombatantSubsystem>();
	if (CombatantSubsystem)
	{
		CombatantSubsystem->RegisterCombatant(this);
	}
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Make sure to check for a valid subsystem - exiting the game was causing a crash because the subsystem
	// was destroyed before the character!
	UCombatantSubsystem* CombatantSubsystem = GetWorld()->GetSubsystem<UCombatantSubsystem>();
	if (CombatantSubsystem)
	{
		// Remove this character from the active combatants
		CombatantSubsystem->UnregisterCombatant(this);
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Do something
}


void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind our ability system component directly to the player input
	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent,
		FGameplayAbilityInputBinds("Confirm", "Cancel", "EAbilityInputType",
			static_cast<int32>(EAbilityInputType::Cancel), static_cast<int32>(EAbilityInputType::Confirm)));
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		// Required for the ability system to function properly
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}





// Combatant overrides

TSubclassOf<UTeam> ABaseCharacter::GetTeam() const
{
	return TeamClass;
}

FVector ABaseCharacter::GetTargetLocation() const
{
	if (bUseSocketForTargetLocation)
	{
		const USkeletalMeshSocket* TargetSocket = GetTargetSocket();

		if (TargetSocket)
		{
			// GetTargetSocket() already checks for a valid skeletal mesh, so we know GetMesh() should be good
			return TargetSocket->GetSocketLocation(GetMesh());
		}
	}

	// If bUseSocketForTargetLocation is false, or something about the socket is invalid, just use the relative location
	const FTransform ActorTransform = GetTransform();
	return ActorTransform.TransformPosition(RelativeTargetLocation);
}

void ABaseCharacter::AttachTargetIndicator(AActor* TargetIndicator)
{
	// A sanity check to make sure there actually is a target indicator - only check on development builds
	#if !UE_BUILD_SHIPPING
		if (!TargetIndicator)
		{
			UE_LOG(LogThresholdGeneral, Error, TEXT("Attempting to attach null TargetIndicator to character %s"),
				*GetNameSafe(this))
			return;
		}
	#endif

	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

	if (!SkeletalMeshComponent)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Attaching target indicator %s to character %s - character has no "
			"skeletal mesh, attaching at actor level instead"), *GetNameSafe(TargetIndicator),
			*GetNameSafe(this))

		TargetIndicator->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TargetIndicator->SetActorRelativeLocation(RelativeTargetLocation);

		return;
	}

	if (bUseSocketForTargetLocation)
	{
		const USkeletalMeshSocket* TargetSocket = GetTargetSocket();

		if (TargetSocket)
		{
			TargetSocket->AttachActor(TargetIndicator, SkeletalMeshComponent);
			return;
		}

		UE_LOG(LogThresholdGeneral, Warning, TEXT("Attaching target indicator %s to character %s - socket %s is invalid "
			"using relative offset instead"), *GetNameSafe(TargetIndicator), *GetNameSafe(this),
			*TargetSocketName.ToString())
	}

	TargetIndicator->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TargetIndicator->SetActorRelativeLocation(RelativeTargetLocation);
}




// Accessors

const USkeletalMeshSocket* ABaseCharacter::GetTargetSocket() const
{
	const USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

	// Try to get the socket with lots of checks and warnings
	if (!SkeletalMeshComponent)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Character %s has bUseSocketForTargetLocation  = true "
			"but no valid skeletal mesh component"), *GetNameSafe(this))

		return nullptr;
	}

	const USkeletalMeshSocket* TargetSocket = SkeletalMeshComponent->GetSocketByName(TargetSocketName);

	if (!TargetSocket)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Character %s has bUseSocketForTargetLocation  = true "
                "but socket %s is not valid on skeletal mesh component %s"), *GetNameSafe(this),
                *TargetSocketName.ToString(), *GetNameSafe(SkeletalMeshComponent))
		
		return nullptr;
	}
	
	return TargetSocket;
}

FVector ABaseCharacter::GetLocalMovementVectorScaled() const
{
	const UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (!MovementComponent)
	{
		// We can't be moving if we don't have a movement component!
		return FVector::ZeroVector;
	}

	// Scale our world movement by current max speed
	const FVector WorldMovementVector = GetVelocity() / MovementComponent->GetMaxSpeed();

	// Transform our world space movement into local space movement
	return GetTransform().InverseTransformVector(WorldMovementVector);
}

FVector ABaseCharacter::GetLocalMovementVectorNormalized() const
{
	return GetLocalMovementVectorScaled().GetSafeNormal();
}


FVector ABaseCharacter::GetWorldLookLocation() const
{
	return GetTransform().TransformPosition(RelativeLookLocation);
}

bool ABaseCharacter::GetIsDodging() const
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!MovementComponent)
	{
		return false;
	}

	// TODO: Find a better way of detecting dodge motion
	return MovementComponent->GetRootMotionSource(DodgeRootMotionName).IsValid();
}







// Private helper functions

void ABaseCharacter::GrantStartingAbilities()
{
	// Only grant abilities if we are the server
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || bWasGrantedStartingAbilities)
	{
		return;
	}

	// Give each ability and bind it to the default input binding
	for (TSubclassOf<UTHGameplayAbility>& AbilityClass : StartingAbilities)
	{
		const UTHGameplayAbility* AbilityDefaultObject = AbilityClass.GetDefaultObject();
		
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
            AbilityClass, 1, static_cast<int32>(AbilityDefaultObject->DefaultInputBinding), this));
	}

	bWasGrantedStartingAbilities = true;
}

