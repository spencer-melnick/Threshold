// Copyright © 2020 Spencer Melnick


#include "THCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Threshold/Character/THCharacterMovement.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Controllers/THPlayerController.h"
#include "Threshold/Animation/THCharacterAnim.h"
#include "Threshold/Global/THConfig.h"
#include "Threshold/Combat/WeaponMoveset.h"
#include "Threshold/Combat/DamageTypes.h"
#include "Threshold/Global/Subsystems/CombatantSubsystem.h"
#include "Threshold/Abilities/Motion/THMotionSources.h"
#include "Threshold/Abilities/THGameplayAbility.h"


// FName constants

FName ATHCharacter::DodgeMotionName = TEXT("DodgeMotion");
FName ATHCharacter::AbilitySystemComponentName = TEXT("AbilitySystemComponent");



// Sets default values
ATHCharacter::ATHCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTHCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the third person camera spring arm and copy controller pitch and yaw
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bInheritPitch = true;
	CameraArm->bInheritYaw = true;

	// Create the third person camera
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Third Person Camera"));
	ThirdPersonCamera->SetupAttachment(CameraArm);

	// Disable controller rotation for capsule so it can be set manually as needed
	bUseControllerRotationPitch = false;

	// Attach an ability system component and set it to replicated
	AbilitySystemComponent = CreateDefaultSubobject<UTHAbilitySystemComponent>(AbilitySystemComponentName);
	AbilitySystemComponent->SetIsReplicated(true);
}




// Engine overrides

void ATHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Early exit if there is no movement component
	if (CustomCharacterMovement == nullptr)
	{
		return;
	}

	// Disable movement if current mode is walking and we're not supposed to be
	// able to walk
	if (!GetCanWalk() && CustomCharacterMovement->MovementMode == MOVE_Walking)
	{
		// CustomCharacterMovement->SetMovementMode(MOVE_None);
	}
	else if (GetCanWalk() && CustomCharacterMovement->MovementMode == MOVE_None)
	{
		CustomCharacterMovement->SetMovementMode(MOVE_Walking);
	}

	// Check if character is moving and update rotation
	FVector Velocity = GetVelocity();

	if (Velocity.X != 0.f || Velocity.Y != 0.f && GetCanWalk())
	{
		// Rotate towards the controller rotation
		FRotator ControlRotation = GetControlRotation();
		FRotator DesiredRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
		FRotator NewRotation = FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaTime, CharacterRotationSpeed);
		SetActorRotation(NewRotation);
	}

	// Run hit slowdown code
	ApplyHitSlowdown(DeltaTime);

	if (bIsAttackDamaging)
	{
		SweepWeaponCollision(DeltaTime);
	}
}

void ATHCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Cache components
	CustomCharacterMovement = Cast<UTHCharacterMovement>(GetCharacterMovement());

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh != nullptr)
	{
		CharacterAnim = Cast<UTHCharacterAnim>(SkeletalMesh->GetAnimInstance());
	}
}

float ATHCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	
	UE_LOG(LogTemp, Display, TEXT("%s took %.2f damage from %s"), *GetNameSafe(this),
		DamageAmount, *GetNameSafe(DamageCauser))

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID) && CharacterAnim != nullptr)
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		FVector HitDirection2D = PointDamageEvent.ShotDirection.GetSafeNormal2D();
		FVector2D HitVector(HitDirection2D.X, HitDirection2D.Y);

		// Play the hit reaction animation via the anim blueprint
		CharacterAnim->ReactToHit(HitVector);
	}
	
	if (CurrentHealth <= 0.f)
	{
		OnDeath();
	}
	
	return DamageAmount;
}





// Movement

void ATHCharacter::Dodge(FVector DodgeVector)
{
	// Only dodge if you can actually move
	if (!GetCanDodge() || CustomCharacterMovement == nullptr || DodgePositionCurve == nullptr)
	{
		return;
	}

	// Set our dodge vector
	DodgeDirection = DodgeVector.GetSafeNormal2D();

	// Create a new root motion
	FRootMotionSource_PositionCurve* DodgeMotion = new FRootMotionSource_PositionCurve();
	DodgeMotion->Direction = DodgeDirection;
	DodgeMotion->Scale = DodgeDistance;
	DodgeMotion->Duration = DodgeDuration;
	DodgeMotion->PositionOverTime = DodgePositionCurve;
	DodgeMotion->InstanceName = DodgeMotionName;

	// Apply our root motion and track the ID
	CustomCharacterMovement->ApplyRootMotionSource(DodgeMotion);
}




// Actions

void ATHCharacter::PrimaryAttack()
{
	if (!GetCanAttack())
	{
		return;
	}

	PerformNextAttack(EWeaponMoveType::Primary);
}




// Combat

void ATHCharacter::ChangeActiveWeapon(UPrimitiveComponent* NewWeapon)
{
	// Get rid of old weapon overlap events
	if (ActiveWeapon != nullptr)
	{
		ActiveWeapon->OnComponentBeginOverlap.RemoveDynamic(this, &ATHCharacter::OnWeaponOverlap);
		ActiveWeapon->OnComponentEndOverlap.RemoveDynamic(this, &ATHCharacter::OnWeaponEndOverlap);
	}
	
	ActiveWeapon = NewWeapon;
	ActiveWeapon->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATHCharacter::OnWeaponOverlap);
	ActiveWeapon->OnComponentEndOverlap.AddUniqueDynamic(this, &ATHCharacter::OnWeaponEndOverlap);
}




// Animation responses

void ATHCharacter::OnAttackDamageStart()
{
	// Start doing some damage!
	bIsAttackDamaging = true;

	// Trigger the blueprint event
	OnAttackDamageStartBP();
}

void ATHCharacter::OnAttackDamageEnd()
{
	// Stop doing damage!
	bIsAttackDamaging = false;

	// Trigger the combo window
	bCanComboAttack = true;

	// Trigger the blueprint event
	OnAttackDamageEndBP();
}

void ATHCharacter::OnAttackEnd()
{
	bIsAttacking = false;
	bCanComboAttack = false;

	// Reset active move
	ActiveWeaponMove = nullptr;
}




// Accessors

FVector ATHCharacter::GetHeadPosition() const
{
	return CameraArm->GetComponentLocation();
}

FVector2D ATHCharacter::GetMovementVelocity() const
{
	if (CustomCharacterMovement == nullptr)
	{
		return FVector2D::ZeroVector;
	}
	
	// Return zero to prevent NaN
	float MaxSpeed = CustomCharacterMovement->GetMaxSpeed();
	if (FMath::IsNearlyZero(MaxSpeed))
	{
		return FVector2D::ZeroVector;
	}
	
	FVector ScaledVelocity = GetVelocity() / MaxSpeed;

	FRotator MovementRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector2D MovementVelocity;
	MovementVelocity.X = FVector::DotProduct(ScaledVelocity, MovementRotation.RotateVector(FVector::RightVector));
	MovementVelocity.Y = FVector::DotProduct(ScaledVelocity, MovementRotation.RotateVector(FVector::ForwardVector));

	return MovementVelocity;
}

FVector2D ATHCharacter::GetDodgeDirection(float Threshold) const
{
	FVector2D LocalDodgeDirection;

	LocalDodgeDirection.Y = FVector::DotProduct(DodgeDirection, GetActorForwardVector());
	LocalDodgeDirection.X = FVector::DotProduct(DodgeDirection, GetActorRightVector());

	return LocalDodgeDirection;
}

bool ATHCharacter::GetIsDodging() const
{
	if (!CustomCharacterMovement)
	{
		return false;
	}

	return CustomCharacterMovement->GetRootMotionSource(DodgeMotionName).IsValid();
}

bool ATHCharacter::GetCanWalk() const
{
	return (GetCharacterIsAlive() && !bIsAttacking && !GetIsDodging());
}

bool ATHCharacter::GetCanDodge() const
{
	return false;
	// return (GetCharacterIsAlive() && GetCanWalk() && !GetIsDodging() && !CustomCharacterMovement->IsFalling());
}

bool ATHCharacter::GetCanAttack() const
{
	return GetCharacterIsAlive() && (GetCanWalk() || bCanComboAttack);
}

UTHCharacterAnim* ATHCharacter::GetCharacterAnim() const
{
	return CharacterAnim;
}

UPrimitiveComponent* ATHCharacter::GetActiveWeapon() const
{
	return ActiveWeapon;
}

bool ATHCharacter::GetCharacterIsAlive() const
{
	return LifeState == ECharacterLifeState::Alive;
}





// Interface overrides

TSubclassOf<UTeam> ATHCharacter::GetTeam() const
{
	return Team;
}

bool ATHCharacter::GetCanBeTargeted() const
{
	return GetCharacterIsAlive();
}

bool ATHCharacter::GetCanBeDamaged() const
{
	return GetCharacterIsAlive();
}

FVector ATHCharacter::GetTargetLocation() const
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh == nullptr)
	{
		return FVector::ZeroVector;
	}
	
	return SkeletalMesh->GetSocketLocation(TargetSocketName);
}

void ATHCharacter::AttachTargetIndicator(AActor* ActorToBeAttached)
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (SkeletalMesh == nullptr)
	{
		ActorToBeAttached->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		return;
	}

	ActorToBeAttached->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TargetSocketName);
}




// Called when the game starts or when spawned
void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the current health based on blueprint properties
	CurrentHealth = FMath::Min(StartingHealth, MaxHealth);

	// Register ourself with the combatant subsystem
	GetWorld()->GetSubsystem<UCombatantSubsystem>()->RegisterCombatant(this);

	GrantDefaultAbilities();
}

void ATHCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Unregister self from the combatant system
	GetWorld()->GetSubsystem<UCombatantSubsystem>()->UnregisterCombatant(this);
}

void ATHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent,
		FGameplayAbilityInputBinds("Confirm", "Cancel", "EAbilityInputType",
			static_cast<int32>(EAbilityInputType::Cancel), static_cast<int32>(EAbilityInputType::Confirm)));
}



void ATHCharacter::OnAttackingActor(AActor* OtherActor, FHitResult HitResult, FVector HitVelocity)
{
	TimeSinceLastHit = 0.f;

	// Try to play the hitshake
	PlayScreenShake(HitVelocity.GetSafeNormal());

	// Apply damage
	float Damage = CalculateBaseDamage() * ActiveWeaponMove->DamageScale;
	FPointDamageEvent DamageEvent(Damage, HitResult, HitVelocity.GetSafeNormal(),
		USwordDamage::StaticClass());
	OtherActor->TakeDamage(Damage, DamageEvent, GetController(), this);
	

	OnAttackingActorBP(OtherActor, HitResult, HitVelocity);
}

void ATHCharacter::OnDeath()
{
	UE_LOG(LogTemp, Display, TEXT("%s died"), *GetNameSafe(this));

	// Change our state and disable collisions
	LifeState = ECharacterLifeState::Dead;
	SetActorEnableCollision(false);

	// Call the blueprint event
	OnDeathBP();
}

float ATHCharacter::CalculateBaseDamage()
{
	return 10.f;
}







// Private helpers

void ATHCharacter::PerformNextAttack(EWeaponMoveType MoveType)
{
	if (ActiveWeaponMoveset == nullptr || CharacterAnim == nullptr)
	{
		return;
	}
	
	uint16_t NextMoveIndex = 0;
	
	// If no move is active, we need to find the appropriate starting move
	if (ActiveWeaponMove == nullptr)
	{
		switch (MoveType)
		{
		case EWeaponMoveType::Primary:
			NextMoveIndex = ActiveWeaponMoveset->StartingPrimaryMoveIndex;
			break;

		case EWeaponMoveType::Secondary:
			NextMoveIndex = ActiveWeaponMoveset->StartingSecondaryMoveIndex;
			break;
		}
    }
	// Otherwise just grab the appropriate chained move from our active move (if any)
	else
	{
		switch (MoveType)
		{
		case EWeaponMoveType::Primary:
			if (!ActiveWeaponMove->bHasPrimaryFollowup)
			{
				return;
			}
			NextMoveIndex = ActiveWeaponMove->PrimaryFollowupIndex;
			break;

		case EWeaponMoveType::Secondary:
			if (!ActiveWeaponMove->bHasSecondaryFollowup)
			{
				return;
			}
			NextMoveIndex = ActiveWeaponMove->SecondaryFollowupIndex;
			break;
		}
	}

	if (NextMoveIndex >= ActiveWeaponMoveset->WeaponMoves.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to access out of range move %d in %s"),
			NextMoveIndex, *GetNameSafe(ActiveWeaponMoveset));
		return;
	}
	
	ActiveWeaponMove = &(ActiveWeaponMoveset->WeaponMoves[NextMoveIndex]);

	// Dispatch our next animation to the anim blueprint to let it decide
	// exactly how to play it
	CharacterAnim->Attack(ActiveWeaponMove->Animation);

	// Reset attack states
	ResetAttack();
	bIsAttacking = true;
	bCanComboAttack = false;
}


void ATHCharacter::ResetAttack()
{
	CurrentlyDamagedActors.Empty();
	CurrentlyWeaponOverlappingActors.Empty();
	LastWeaponSweepPositions.Empty();

	TimeSinceLastHit = 0.f;
}

void ATHCharacter::ApplyHitSlowdown(float DeltaTime)
{
	// Reset time dilation if we aren't currently attacking or damaging anyone
	// (or data is missing!)
	if (!bIsAttacking || HitSlowdownCurve == nullptr || CurrentlyDamagedActors.Num() == 0)
	{
		CustomTimeDilation = 1.f;
		return;
	}

	// Apply undilated delta time here to avoid exponential effects
	// on the slowdown curve
	TimeSinceLastHit += DeltaTime / CustomTimeDilation;

	// If we're no longer overlapping a damageable actor and the minimum
	// hit time has elapsed, we should stop the hit slowdown
	if (CurrentlyWeaponOverlappingActors.Num() == 0 && TimeSinceLastHit >= MinHitSlowdownTime)
	{
		CustomTimeDilation = 1.f;
		return;
	}

	// Stop evaluating curve at max time for performance
	if (TimeSinceLastHit >= MaxHitSlowdownTime)
	{
		TimeSinceLastHit = MaxHitSlowdownTime;
	}

	CustomTimeDilation = HitSlowdownCurve->GetFloatValue(TimeSinceLastHit);
}

void ATHCharacter::PlayScreenShake(FVector ShakeDirection)
{
	// Play the hitshake if we're player controlled
	if (IsPlayerControlled() && HasAuthority())
	{
		ATHPlayerController* PlayerController = Cast<ATHPlayerController>(GetController());
		PlayerController->ApplyHitShake(ShakeDirection, HitShakeAmplitude);
	}
}


void ATHCharacter::SweepWeaponCollision(float DeltaTime)
{
	// Don't do anything if we don't have any sockets to check
	// or no team to determine if we can damage any actors
	if (WeaponSweepSockets.Num() == 0 || Team == nullptr)
	{
		return;
	}

	// Calculate the current positions of all weapon sockets
	TArray<FVector> NewWeaponSweepPositions;
	NewWeaponSweepPositions.SetNumUninitialized(WeaponSweepSockets.Num());

	for (int32 i = 0; i < WeaponSweepSockets.Num(); i++)
	{
		NewWeaponSweepPositions[i] = ActiveWeapon->GetSocketLocation(WeaponSweepSockets[i]);
	}
	
	// Only perform a sweep if we know the last position of
	// our weapon sockets and double check for a mismatch
	// between the number of positions and sockets
	if (LastWeaponSweepPositions.Num() > 0 && LastWeaponSweepPositions.Num() == WeaponSweepSockets.Num())
	{
		FCollisionShape SweepShape = FCollisionShape::MakeSphere(WeaponSweepRadius);
			
		for (int32 i = 0; i < LastWeaponSweepPositions.Num(); i++)
		{
			FVector TraceStart = LastWeaponSweepPositions[i];
			FVector TraceEnd = NewWeaponSweepPositions[i];

			TArray<FHitResult> HitResults;

			if (GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, FQuat::Identity,
				WeaponSweepChannel, SweepShape))
			{
				for (FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();
					ICombatant* HitCombatant = Cast<ICombatant>(HitActor);

					// Only damage actors with the correct team who we haven't hit yet
					if (HitCombatant != nullptr && HitCombatant->GetCanBeDamagedBy(Team) &&
						!CurrentlyDamagedActors.Contains(HitActor))
					{
						// Track hit actor to prevent duplicate hits
						CurrentlyDamagedActors.Add(HitActor);
						
						// Estimate hit velocity based on socket location
						FVector HitVelocity = (TraceEnd - TraceStart) / DeltaTime;
						
						// Exit loop since we've found an actor to damage
						OnAttackingActor(HitResult.GetActor(), HitResult, HitVelocity);
						break;
					}
				}
			}
		}
	}

	// Update our weapon positions using move to avoid copying
	LastWeaponSweepPositions = MoveTemp(NewWeaponSweepPositions);
}

void ATHCharacter::GrantDefaultAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
	{
		return;
	}

	for (TSubclassOf<UTHGameplayAbility>& AbilityClass : StartingAbilities)
	{
		const UTHGameplayAbility* AbilityDefaultObject = AbilityClass.GetDefaultObject();
		
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			AbilityClass, 1, static_cast<int32>(AbilityDefaultObject->DefaultInputBinding), this));
	}
}







// Delegates

void ATHCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only track actors with the correct tag while attack is damaging
	if (!OtherActor->ActorHasTag(DamageableActorTag) || !bIsAttackDamaging)
	{
		return;
	}

	// Keep a list of the overlapping actors
	CurrentlyWeaponOverlappingActors.AddUnique(OtherActor);
}

void ATHCharacter::OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Only track actors with the correct tag
	if (!OtherActor->ActorHasTag(DamageableActorTag))
	{
		return;
	}

	CurrentlyWeaponOverlappingActors.Remove(OtherActor);
}

