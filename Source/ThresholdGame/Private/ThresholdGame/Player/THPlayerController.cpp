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
		// Spawn a target indicator actor
		if (TargetIndicatorClass != nullptr)
		{
			TargetIndicatorActor = GetWorld()->SpawnActor(TargetIndicatorClass);
			TargetIndicatorActor->SetActorHiddenInGame(true);
		}

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
}

void ATHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/*
	InputComponent->BindAction("ToggleTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::ToggleTarget);
	InputComponent->BindAction("NextTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::NextTarget);
	InputComponent->BindAction("PreviousTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::PreviousTarget);
	*/

	InputComponent->BindAction("ToggleMenu", EInputEvent::IE_Pressed, this, &ATHPlayerController::ToggleMenu);
}

void ATHPlayerController::Tick(float DeltaTime)
{
	if (IsLocalController())
	{
		RotateTowardsTarget(DeltaTime);

		// Try to unfollow our target if it's no longer targetable
		if (LockonTarget.IsValid() && !LockonTarget->GetCanBeTargeted())
		{
			SetTarget(nullptr);
		}

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

TArray<ATHPlayerController::FTarget> ATHPlayerController::GetLockonTargets()
{
	TArray<FTarget> Targets;

	// Check for possessed character
	ABaseCharacter* PossessedCharacter = GetBaseCharacter();
	
	if (!PossessedCharacter)
	{
		return Targets;
	}

	const TSubclassOf<UTeam> TeamClass = PossessedCharacter->GetTeam();
	int32 ViewportWidth, ViewportHeight;
	GetViewportSize(ViewportWidth, ViewportHeight);
	const FVector2D ViewportSize(static_cast<float>(ViewportWidth), static_cast<float>(ViewportHeight));
	
	// Iterate through all actors
	for (const TWeakInterfacePtr<ICombatant>& Combatant : GetWorld()->GetSubsystem<UCombatantSubsystem>()->GetCombatants())
	{
		// Only check valid actors belonging to the appropriate teams
		if (!Combatant.IsValid() || !Combatant->GetCanBeTargetedBy(TeamClass))
		{
			continue;
		}

		FTarget Target;
		Target.Combatant = Combatant;

		// Limit targets by distance to possessed character
		Target.Distance = FVector::Distance(Combatant->GetTargetLocation(),
			PossessedCharacter->GetWorldLookLocation());

		if (Target.Distance > MaxTargetDistance)
		{
			continue;
		}

		// Only check actors that are visible on screen
		if (!ProjectWorldLocationToScreen(Combatant->GetTargetLocation(), Target.ScreenPosition) ||
			Target.ScreenPosition < FVector2D::ZeroVector || Target.ScreenPosition > ViewportSize)
		{
			continue;
		}

		Targets.Add(Target);
	}

	return Targets;
}

TArray<ATHPlayerController::FTarget> ATHPlayerController::GetSortedLockonTargets()
{
	// Sort targets by their distance to the character
	TArray<FTarget> PotentialTargets = GetLockonTargets();
	PotentialTargets.Sort([](const FTarget& TargetA, const FTarget& TargetB)
	{
		return (TargetA.Distance < TargetB.Distance);
	});

	return PotentialTargets;
}

TArray<ATHPlayerController::FTarget> ATHPlayerController::GetSortedLockonTargets(int32& CurrentTargetIndex)
{
	// Check to see if the current target isn't included in potential targets
	// (an edge case)
	TArray<FTarget> PotentialTargets = GetLockonTargets();
	FTarget* FoundTarget = PotentialTargets.FindByPredicate([this](const FTarget& Target)
	{
		return LockonTarget == Target.Combatant;
	});

	FTarget CurrentTarget;

	if (FoundTarget == nullptr)
	{
		// Add current target to full list
		CurrentTarget.Combatant = LockonTarget;
		CurrentTarget.ScreenPosition = FVector2D::ZeroVector;
		CurrentTarget.Distance = FVector::Distance(LockonTarget->GetTargetLocation(),
			GetCharacter()->GetActorLocation());

		PotentialTargets.Add(CurrentTarget);
	}
	else
	{
		CurrentTarget = *FoundTarget;
	}

	// Sort targets by their distance to the character
	PotentialTargets.Sort([](const FTarget& TargetA, const FTarget& TargetB)
	{
		return (TargetA.ScreenPosition.X < TargetB.ScreenPosition.X);
	});

	CurrentTargetIndex = PotentialTargets.Find(CurrentTarget);

	return PotentialTargets;
}


void ATHPlayerController::RotateTowardsTarget(float DeltaTime)
{
	const ABaseCharacter* PossessedCharacter = GetBaseCharacter();
	
	if (LockonTarget.IsValid() && PossessedCharacter)
	{
		// Calculate pitch and yaw based on distance to target
		const FVector LookVector = LockonTarget->GetTargetLocation() - PossessedCharacter->GetWorldLookLocation();

		// Rotate towards lockon target
		const FRotator DesiredRotation = LookVector.Rotation() + LockonOffsetRotation;
		const FRotator NewRotation = FMath::RInterpTo(GetControlRotation(), DesiredRotation, DeltaTime, LockonRotationSpeed);
		SetControlRotation(NewRotation);
	}
}

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





// Virtual functions

bool ATHPlayerController::GetCameraIsDirectlyControlled()
{
	if (LockonTarget.IsValid())
	{
		return false;
	}

	return true;
}




// Camera controls

void ATHPlayerController::ToggleTarget()
{
	if (LockonTarget.IsValid())
	{
		SetTarget(TWeakInterfacePtr<ICombatant>());
		return;
	}
	
	TArray<FTarget> PotentialTargets = GetSortedLockonTargets();

	if (PotentialTargets.Num() != 0)
	{
		SetTarget(PotentialTargets[0].Combatant);
	}
}

void ATHPlayerController::NextTarget()
{
	// Skip if the player isn't locked on to a target
	if (!LockonTarget.IsValid())
	{
		return;
	}

	// Pick the next target from the targets sorted by distance
	int32 CurrentTargetIndex;
	TArray<FTarget> PotentialTargets = GetSortedLockonTargets(CurrentTargetIndex);
	CurrentTargetIndex = (CurrentTargetIndex + 1) % PotentialTargets.Num();
	
	SetTarget(PotentialTargets[CurrentTargetIndex].Combatant);
}

void ATHPlayerController::PreviousTarget()
{
	// Skip if the player isn't locked on to a target
	if (!LockonTarget.IsValid())
	{
		return;
	}

	// Pick the previous target from the targets sorted by distance
	int32 CurrentTargetIndex;
	TArray<FTarget> PotentialTargets = GetSortedLockonTargets(CurrentTargetIndex);
	if (CurrentTargetIndex == 0)
	{
		CurrentTargetIndex = PotentialTargets.Num() - 1;
	}
	else
	{
		CurrentTargetIndex--;
	}
	
	SetTarget(PotentialTargets[CurrentTargetIndex].Combatant);
}


void ATHPlayerController::SetTarget(TWeakInterfacePtr<ICombatant> NewTarget)
{
	LockonTarget = NewTarget;

	if (!LockonTarget.IsValid())
	{
		// Detach the target indicator and hide it
		TargetIndicatorActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetIndicatorActor->SetActorHiddenInGame(true);
		return;
	}

	// Try to use the targetable interface to attach the indicator to the target
	LockonTarget->AttachTargetIndicator(TargetIndicatorActor);
	TargetIndicatorActor->SetActorHiddenInGame(false);
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




// Struct operators

bool ATHPlayerController::FTarget::operator==(const FTarget& OtherTarget) const
{
	return Combatant == OtherTarget.Combatant;
}
