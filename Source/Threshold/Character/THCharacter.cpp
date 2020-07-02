// Copyright � 2020 Spencer Melnick


#include "THCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ATHCharacter::ATHCharacter()
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

	if (Velocity.X != 0.f || Velocity.Y != 0.f)
	{
		// Rotate towards the controller rotation
		FRotator ControlRotation = GetControlRotation();
		FRotator DesiredRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
		FRotator NewRotation = FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaTime, CharacterRotationSpeed);
		SetActorRotation(NewRotation);
	}

}



// Accessors

const FVector ATHCharacter::GetHeadPosition() const
{
	return CameraArm->GetComponentLocation();
}







// Called when the game starts or when spawned
void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}



