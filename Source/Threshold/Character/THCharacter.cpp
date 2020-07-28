// Copyright � 2020 Spencer Melnick


#include "THCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Threshold/Character/THCharacterMovement.h"
#include "Threshold/Animation/THCharacterAnim.h"
#include "Threshold/Combat/WeaponMoveset.h"

#include "DrawDebugHelpers.h"

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

	if (bCanComboAttack)
	{
		DrawDebugSphere(GetWorld(), GetHeadPosition(), 10.f, 32, FColor::Cyan);
	}

	// Run hit slowdown code
	ApplyHitSlowdown(DeltaTime);
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




// Movement

void ATHCharacter::Dodge()
{
	// Only dodge if you can actually move
	if (!GetCanDodge())
	{
		return;
	}

	// Rotate to face control before dodging
	FRotator ControlRotation = GetControlRotation();
	FRotator NewRotation(0.f, ControlRotation.Yaw, 0.f);
	// SetActorRotation(NewRotation);

	// Set the movement mode
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, ETHCustomMovementTypes::CUSTOMMOVE_Dodge);
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
}

void ATHCharacter::OnAttackDamageEnd()
{
	// Stop doing damage!
	bIsAttackDamaging = false;

	// Trigger the combo window
	bCanComboAttack = true;
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
	if (CustomCharacterMovement == nullptr)
	{
		return FVector2D::ZeroVector;
	}

	FVector DodgeVector = CustomCharacterMovement->GetDodgeVector();
	FVector2D DodgeVector2D;
	DodgeVector2D.X = FVector::DotProduct(DodgeVector, GetActorRightVector());
	DodgeVector2D.Y = FVector::DotProduct(DodgeVector, GetActorForwardVector());
	
	return DodgeVector2D;
}

bool ATHCharacter::GetIsDodging() const
{
	if (CustomCharacterMovement == nullptr)
	{
		return false;
	}
	
	return (CustomCharacterMovement->MovementMode == MOVE_Custom && 
		CustomCharacterMovement->CustomMovementMode == CUSTOMMOVE_Dodge);
}

bool ATHCharacter::GetCanWalk() const
{
	if (GetIsDodging())
	{
		return false;
	}

	if (bIsAttacking)
	{
		return false;
	}

	return true;
}

bool ATHCharacter::GetCanDodge() const
{
	return (GetCanWalk() && !GetIsDodging() && !CustomCharacterMovement->IsFalling());
}

bool ATHCharacter::GetCanAttack() const
{
	return GetCanWalk() || bCanComboAttack;
}

UTHCharacterAnim* ATHCharacter::GetCharacterAnim() const
{
	return CharacterAnim;
}

UPrimitiveComponent* ATHCharacter::GetActiveWeapon() const
{
	return ActiveWeapon;
}




// Called when the game starts or when spawned
void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATHCharacter::OnAttackingActor(AActor* OtherActor)
{
	TimeSinceLastHit = 0.f;
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

	UE_LOG(LogTemp, Display, TEXT("Performing move %d on %s"),
		NextMoveIndex, *GetNameSafe(ActiveWeaponMoveset));
}


void ATHCharacter::ResetAttack()
{
	CurrentlyDamagedActors.Empty();
	CurrentlyWeaponOverlappingActors.Empty();

	TimeSinceLastHit = 0.f;
}

void ATHCharacter::ApplyHitSlowdown(float DeltaTime)
{
	// Reset time dilation
	if (!bIsAttacking || HitSlowdownCurve == nullptr || CurrentlyWeaponOverlappingActors.Num() == 0)
	{
		CustomTimeDilation = 1.f;
		return;
	}

	// Apply undilated delta time here to avoid exponential effects
	// on the slowdown curve
	TimeSinceLastHit += DeltaTime / CustomTimeDilation;

	// Stop evaluating curve at max time for performance
	if (TimeSinceLastHit >= MaxHitSlowdownTime)
	{
		TimeSinceLastHit = MaxHitSlowdownTime;
	}

	CustomTimeDilation = HitSlowdownCurve->GetFloatValue(TimeSinceLastHit);
}





// Delegates

void ATHCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only damage actors with the correct tag while attack is damaging
	if (!OtherActor->ActorHasTag(DamageableActorTag) || !bIsAttackDamaging)
	{
		return;
	}

	// Keep a list of the overlapping actors
	CurrentlyWeaponOverlappingActors.AddUnique(OtherActor);

	// Skip actors we're already damaging
	if (CurrentlyDamagedActors.Contains(OtherActor))
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("%s hit %s with a weapon"), *GetNameSafe(this), *GetNameSafe(OtherActor));
	CurrentlyDamagedActors.Add(OtherActor);
	OnAttackingActor(OtherActor);
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

