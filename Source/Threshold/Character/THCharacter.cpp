// Copyright © 2020 Spencer Melnick


#include "THCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Threshold/Character/THCharacterMovement.h"
#include "Threshold/Animation/THCharacterAnim.h"

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
}

void ATHCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Cache components
	CustomCharacterMovement = Cast<UTHCharacterMovement>(GetCharacterMovement());
}







// Movement

void ATHCharacter::Dodge()
{
	// Only dodge if you can actually move
	if (!GetCanWalk())
	{
		return;
	}
	
	UTHCharacterAnim* CharacterAnim = GetCharacterAnim();

	// Trigger the dodge animation
	if (CharacterAnim != nullptr)
	{
		CharacterAnim->Dodge();
	}

	// Rotate to face control before dodging
	FRotator ControlRotation = GetControlRotation();
	FRotator NewRotation(0.f, ControlRotation.Yaw, 0.f);
	SetActorRotation(NewRotation);

	// Set the movement mode
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, ETHCustomMovementTypes::CUSTOMMOVE_Dodge);
}

void ATHCharacter::EndDodge()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}








// Accessors

FVector ATHCharacter::GetHeadPosition() const
{
	return CameraArm->GetComponentLocation();
}

FVector2D ATHCharacter::GetMovementVelocity() const
{
	FVector ScaledVelocity = GetVelocity() / GetCharacterMovement()->GetMaxSpeed();
	FVector2D MovementVelocity;
	MovementVelocity.X = FVector::DotProduct(ScaledVelocity, GetActorRightVector());
	MovementVelocity.Y = FVector::DotProduct(ScaledVelocity, GetActorForwardVector());

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


bool ATHCharacter::GetCanWalk() const
{
	if (CustomCharacterMovement->MovementMode == MOVE_Custom && 
		CustomCharacterMovement->CustomMovementMode == CUSTOMMOVE_Dodge)
	{
		return false;
	}

	return true;
}

UTHCharacterAnim* ATHCharacter::GetCharacterAnim() const
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh == nullptr)
	{
		return nullptr;
	}

	return Cast<UTHCharacterAnim>(SkeletalMesh->GetAnimInstance());
}










// Called when the game starts or when spawned
void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}



