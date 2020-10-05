// Copyright (c) 2020 Spencer Melnick

#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Player/THPlayerState.h"


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

	// Replicate gameplay effects to the owning client - suggested for player controlled characters
	GetTHAbilitySystemComponent()->ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	// Enable input buffering on the ability system component
	GetTHAbilitySystemComponent()->bEnableInputBuffering = true;
}




// Engine overrides

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	const UTHAbilitySystemComponent* ASC = GetTHAbilitySystemComponent();
	if (ASC && ASC->HasMatchingGameplayTag(DamagingTag))
	{
		// Do something
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}



// Inventory owner overrides

UInventoryComponent* APlayerCharacter::GetInventoryComponent() const
{
	ATHPlayerState* THPlayerState = GetPlayerStateChecked<ATHPlayerState>();

	if (!THPlayerState)
	{
		return nullptr;
	}

	return THPlayerState->GetInventoryComponent();
}

