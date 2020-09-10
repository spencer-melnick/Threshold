// Copyright (c) 2020 Spencer Melnick


#include "THPlayerController.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Character/THCharacter.h"
#include "Threshold/Effects/Camera/THPlayerCameraManager.h"
#include "Threshold/Global/Subsystems/CombatantSubsystem.h"
#include "EngineUtils.h"



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
	// Spawn a target indicator actor
	if (TargetIndicatorClass != nullptr)
	{
		TargetIndicatorActor = GetWorld()->SpawnActor(TargetIndicatorClass);
		TargetIndicatorActor->SetActorHiddenInGame(true);
	}
}



void ATHPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ATHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATHPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATHPlayerController::MoveRight);

	InputComponent->BindAxis("LookUp", this, &ATHPlayerController::LookUp);
    InputComponent->BindAxis("Turn", this, &ATHPlayerController::Turn);

	InputComponent->BindAction("ToggleTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::ToggleTarget);
	InputComponent->BindAction("NextTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::NextTarget);
	InputComponent->BindAction("PreviousTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::PreviousTarget);
}

void ATHPlayerController::Tick(float DeltaTime)
{
	RotateTowardsTarget(DeltaTime);

	// Try to unfollow our target if it's no longer targetable
	if (LockonTarget != nullptr && !LockonTarget->GetCanBeTargeted())
	{
		SetTarget(nullptr);
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
	for (TScriptInterface<ICombatant> Combatant : GetWorld()->GetSubsystem<UCombatantSubsystem>()->GetCombatants())
	{
		// Only check actors belonging to the appropriate teams
		if (!Combatant->GetCanBeTargetedBy(TeamClass))
		{
			continue;
		}

		FTarget Target;
		Target.TargetActor = &*Combatant;

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
		return (&*LockonTarget) == Target.TargetActor;
	});

	FTarget CurrentTarget;

	if (FoundTarget == nullptr)
	{
		// Add current target to full list
		CurrentTarget.TargetActor = &(*LockonTarget);
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
	
	if (LockonTarget && PossessedCharacter)
	{
		// Calculate pitch and yaw based on distance to target
		FVector LookVector = LockonTarget->GetTargetLocation() - PossessedCharacter->GetWorldLookLocation();

		// Rotate towards lockon target
		FRotator DesiredRotation = LookVector.Rotation() + LockonOffsetRotation;
		FRotator NewRotation = FMath::RInterpTo(GetControlRotation(), DesiredRotation, DeltaTime, LockonRotationSpeed);
		SetControlRotation(NewRotation);
	}
}





// Virtual functions

bool ATHPlayerController::GetCameraIsDirectlyControlled()
{
	if (LockonTarget != nullptr)
	{
		return false;
	}

	return true;
}







// Movement 

void ATHPlayerController::MoveForward(float Scale)
{
	ACharacter* PossessedCharacter = GetCharacter();
	if (PossessedCharacter == nullptr)
	{
		return;
	}

	FRotator MovementRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector MovementBasis = MovementRotator.RotateVector(FVector::ForwardVector);
	PossessedCharacter->AddMovementInput(MovementBasis * Scale);
}

void ATHPlayerController::MoveRight(float Scale)
{
	ACharacter* PossessedCharacter = GetCharacter();
	if (PossessedCharacter == nullptr)
	{
		return;
	}
	
	FRotator MovementRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector MovementBasis = MovementRotator.RotateVector(FVector::RightVector);
	PossessedCharacter->AddMovementInput(MovementBasis * Scale);
}




// Camera controls

void ATHPlayerController::LookUp(float Scale)
{
	if (GetCameraIsDirectlyControlled())
	{
		AddPitchInput(Scale);
	}
}


void ATHPlayerController::Turn(float Scale)
{
	if (GetCameraIsDirectlyControlled())
	{
		AddYawInput(Scale);
	}
}



void ATHPlayerController::ToggleTarget()
{
	if (LockonTarget != nullptr)
	{
		SetTarget(nullptr);
		return;
	}
	
	TArray<FTarget> PotentialTargets = GetSortedLockonTargets();

	if (PotentialTargets.Num() != 0)
	{
		SetTarget(PotentialTargets[0].TargetActor);
	}
}

void ATHPlayerController::NextTarget()
{
	// Skip if the player isn't locked on to a target
	if (LockonTarget == nullptr)
	{
		return;
	}

	// Pick the next target from the targets sorted by distance
	int32 CurrentTargetIndex;
	TArray<FTarget> PotentialTargets = GetSortedLockonTargets(CurrentTargetIndex);
	CurrentTargetIndex = (CurrentTargetIndex + 1) % PotentialTargets.Num();
	
	SetTarget(PotentialTargets[CurrentTargetIndex].TargetActor);
}

void ATHPlayerController::PreviousTarget()
{
	// Skip if the player isn't locked on to a target
	if (LockonTarget == nullptr)
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
	
	SetTarget(PotentialTargets[CurrentTargetIndex].TargetActor);
}


void ATHPlayerController::SetTarget(ICombatant* NewTarget)
{
	LockonTarget = Cast<UObject>(NewTarget);

	if (LockonTarget == nullptr)
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
	return TargetActor == OtherTarget.TargetActor;
}
