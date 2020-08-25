// Copyright (c) 2020 Spencer Melnick

#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"




// Component name constants

FName APlayerCharacter::SpringArmComponentName(TEXT("SpringArmComponent"));
FName APlayerCharacter::ThirdPersonCameraComponentName(TEXT("ThridPersonCameraComponent"));




// Default constructor

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create our spring arm and attach it to our root component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(SpringArmComponentName);
	SpringArmComponent->SetupAttachment(RootComponent);

	// Create our third person camera and attach it to the spring arm
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(ThirdPersonCameraComponentName);
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent);

	// Set defaults so the controller drives our rotation properly
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
}

