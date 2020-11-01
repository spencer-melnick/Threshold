// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppMemberFunctionMayBeConst

#include "ThresholdGame/Player/THPlayerController.h"
#include "GameFramework/HUD.h"
#include "ThresholdGame/Abilities/THAbilitySystemComponent.h"
#include "ThresholdGame/Character/THCharacter.h"
#include "ThresholdGame/Effects/Camera/THPlayerCameraManager.h"
#include "ThresholdGame/Global/Subsystems/CombatantSubsystem.h"
#include "ThresholdGame/Global/Subsystems/InteractionSubsystem.h"
#include "ThresholdGame/World/InteractiveObject.h"
#include "ThresholdGame/Player/HUDControl.h"
#include "EngineUtils.h"



// ATHPlayerController

ATHPlayerController::ATHPlayerController()
{
	// Tick every update
	PrimaryActorTick.bCanEverTick = true;

	// Change default camera manager
	PlayerCameraManagerClass = ATHPlayerCameraManager::StaticClass();
}


// Engine overrides

void ATHPlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		// Spawn an interaction indicator actor
		if (InteractionIndicatorClass)
		{
			InteractionIndicatorActor = GetWorld()->SpawnActor(InteractionIndicatorClass);
			InteractionIndicatorActor->SetActorHiddenInGame(true);
		}
	}
}



void ATHPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Try to set our new pawn's input status to our input status
	if (!InPawn)
	{
		return;
	}
	if (bPawnInputEnabled)
	{
		InPawn->EnableInput(this);
	}
	else
	{
		InPawn->DisableInput(this);
	}
}

void ATHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ToggleMenu", EInputEvent::IE_Pressed, this, &ATHPlayerController::ToggleMenu);
}

void ATHPlayerController::Tick(float DeltaTime)
{
	if (IsLocalController())
	{
		CheckInteractiveObjects();
	}
}

void ATHPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	ABaseCharacter* PossessedCharacter = Cast<ABaseCharacter>(P);
	if (PossessedCharacter)
	{
		PossessedCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(PossessedCharacter, PossessedCharacter);
	}
}

void ATHPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	if (!IsNetMode(NM_Client) && IsLocalPlayerController())
	{
		// If this is a server, and the player controller is local, it's either a bot or we're a listen server
		
		InitializePlayerStateUI();
	}
}

void ATHPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		InitializePlayerStateUI();
	}
}





// Helper functions

void ATHPlayerController::CheckInteractiveObjects()
{
	UInteractionSubsystem* InteractionSubsystem = GetWorld()->GetSubsystem<UInteractionSubsystem>();
	ABaseCharacter* BaseCharacter = GetBaseCharacter();

	if (!InteractionSubsystem || !BaseCharacter)
	{
		// Quick check for our character and interaction subsystem
		return;
	}

	float ClosestDistanceSquared = 0.f;
	TWeakInterfacePtr<IInteractiveObject> ClosestObject;

	for (const TWeakInterfacePtr<IInteractiveObject>& InteractiveObject : InteractionSubsystem->GetObjects())
	{
		if (!InteractiveObject.IsValid() || !InteractiveObject->CanInteract(GetBaseCharacter()))
		{
			// Skip invalid objects (we need to check since these are weak pointers) and objects with interaction disabled
			continue;
		}

		// Use squared distance to speed up calculations a little bit
		const float DistanceSquared = (BaseCharacter->GetActorLocation() - InteractiveObject->GetInteractLocation()).SizeSquared();

		// We are always the closest if no other object has been found yet
		const bool bInRange = DistanceSquared < (MaxInteractionDistance * MaxInteractionDistance);
		const bool bIsClosest = !ClosestObject.IsValid() || DistanceSquared < ClosestDistanceSquared;

		if (bInRange && bIsClosest)
		{
			ClosestDistanceSquared = DistanceSquared;
			ClosestObject = InteractiveObject;
		}
	}

	if (ClosestObject != CurrentInteractiveObject)
	{
		// If we changed what the closest interactive object is (even if the new closest interactive object is null)
		SetCurrentInteractiveObject(ClosestObject);
	}
}

void ATHPlayerController::SetCurrentInteractiveObject(TWeakInterfacePtr<IInteractiveObject> NewObject)
{
	if (NewObject == CurrentInteractiveObject)
	{
		// Early exit if the new interactive object didn't change
		return;
	}

	CurrentInteractiveObject = NewObject;

	if (!InteractionIndicatorActor)
	{
		// If we don't have an indicator, skip the indicator logic
		return;
	}

	if (CurrentInteractiveObject.IsValid())
	{
		// If our new interface is valid, attach it to our interactive object and show it
		CurrentInteractiveObject->AttachInteractionIndicator(InteractionIndicatorActor);
		InteractionIndicatorActor->SetActorHiddenInGame(false);
	}
	else
	{
		// Otherwise detach the indicator and hide it
		InteractionIndicatorActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		InteractionIndicatorActor->SetActorHiddenInGame(true);
	}
}



// HUD controls

void ATHPlayerController::ToggleMenu()
{
	IHUDControl* PlayerHUD = GetHUD<IHUDControl>();

	if (!PlayerHUD)
	{
		return;
	}

	switch (PlayerHUD->GetStatus())
	{
		case EPlayerHUDStatus::WorldView:
			PlayerHUD->SetStatus(EPlayerHUDStatus::PlayerMenuActive);
			break;

		default:
			PlayerHUD->SetStatus(EPlayerHUDStatus::WorldView);
			break;
	}

	SetPawnInputEnabled(PlayerHUD->ShouldEnableCharacterControl());
}

void ATHPlayerController::InitializePlayerStateUI()
{
	IHUDControl* PlayerHUD = GetHUD<IHUDControl>();
		
	if (PlayerHUD)
	{
		PlayerHUD->OnPlayerStateInitialized();
	}
}






// Camera effects

void ATHPlayerController::ApplyHitShake(FVector Direction, float Amplitude)
{
	ATHPlayerCameraManager* THCameraManager = GetTHPlayerCameraManager();
	
	if (THCameraManager == nullptr || THCameraManager->GetHitShakeModifier() == nullptr)
	{
		return;
	}

	THCameraManager->ApplyHitShake(Direction, Amplitude, HitShakeDuration, HitShakeCurve);
}



// Input controls

void ATHPlayerController::SetPawnInputEnabled(const bool bNewPawnInputEnabled)
{
	if (bNewPawnInputEnabled == bPawnInputEnabled)
	{
		return;
	}

	bPawnInputEnabled = bNewPawnInputEnabled;
	
	APawn* ControlledPawn = GetPawn();
	if (bPawnInputEnabled)
	{
		if (ControlledPawn)
		{
			ControlledPawn->EnableInput(this);
		}

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		if (ControlledPawn)
		{
			ControlledPawn->DisableInput(this);
		}
	}
}
