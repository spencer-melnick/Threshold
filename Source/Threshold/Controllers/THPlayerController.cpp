// Copyright � 2020 Spencer Melnick


#include "THPlayerController.h"

#include "Threshold/Character/THCharacter.h"
#include "Threshold/Controllers/THPlayerCameraManager.h"
#include "Threshold/Combat/HitCameraShake.h"
#include "EngineUtils.h"


ATHPlayerController::ATHPlayerController()
	: InputBuffer(InputBufferSize + 1)
{
	// Tick every update
	PrimaryActorTick.bCanEverTick = true;

	// Change default camera manager
	PlayerCameraManagerClass = ATHPlayerCameraManager::StaticClass();
}

ATHPlayerController::ATHPlayerController(FVTableHelper& Helper)
	: Super(Helper), InputBuffer(InputBufferSize + 1)
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

	// Try to cast the camera manager
	CustomCameraManager = Cast<ATHPlayerCameraManager>(PlayerCameraManager);
}



void ATHPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cache character reference
	PossessedCharacter = Cast<ATHCharacter>(InPawn);
	UE_LOG(LogTemp, Display, TEXT("%s possessing character"), *GetNameSafe(this));
}

void ATHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATHPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATHPlayerController::MoveRight);

	InputComponent->BindAction("PrimaryAttack", EInputEvent::IE_Released, this, &ATHPlayerController::PrimaryAttack);

	InputComponent->BindAction("Dodge", EInputEvent::IE_Pressed, this, &ATHPlayerController::Dodge);

	InputComponent->BindAxis("LookUp", this, &ATHPlayerController::LookUp);
    InputComponent->BindAxis("Turn", this, &ATHPlayerController::Turn);

	InputComponent->BindAction("ToggleTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::ToggleTarget);
	InputComponent->BindAction("NextTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::NextTarget);
	InputComponent->BindAction("PreviousTarget", EInputEvent::IE_Pressed, this, &ATHPlayerController::PreviousTarget);
}

void ATHPlayerController::Tick(float DeltaTime)
{
	if (PossessedCharacter == nullptr)
	{
		return;
	}

	RotateTowardsTarget(DeltaTime);

	// Check the most recently buffered input
	while (!InputBuffer.IsEmpty())
	{
		const FBufferedInput* MostRecentInput = InputBuffer.Peek();

		// Check if the most recent input has expired
		if (GetWorld()->GetRealTimeSeconds() - MostRecentInput->BufferedTime > ActionBufferTime)
		{
			// Remove the most recent input and check the next one
			InputBuffer.Dequeue();
			continue;
		}

		// Try to consume the input
		if (TryConsumePlayerInput(MostRecentInput))
		{
			InputBuffer.Dequeue();
			continue;
		}

		break;
	}
}



// Helper functions

TArray<ATHPlayerController::FTarget> ATHPlayerController::GetLockonTargets()
{
	TArray<FTarget> Targets;

	// Check for possessed character
	if (PossessedCharacter == nullptr || PossessedCharacter->Team == nullptr)
	{
		return Targets;
	}

	int32 ViewportWidth, ViewportHeight;
	GetViewportSize(ViewportWidth, ViewportHeight);
	FVector2D ViewportSize(static_cast<float>(ViewportWidth), static_cast<float>(ViewportHeight));
	
	// Iterate through all actors
	for (TActorIterator<AActor> TargetIterator(GetWorld()); TargetIterator; ++TargetIterator)
	{
		ITeamMember* TargetTeamMember = Cast<ITeamMember>(*TargetIterator);

		// Only check actors belonging to the appropriate teams
		if (TargetTeamMember == nullptr || !TargetTeamMember->GetCanBeTargetedBy(PossessedCharacter->Team))
		{
			continue;
		}

		FTarget Target;
		Target.TargetActor = *TargetIterator;

		// Limit targets by distance to possessed character
		Target.Distance = Target.TargetActor->GetDistanceTo(PossessedCharacter);

		if (Target.Distance > MaxTargetDistance)
		{
			continue;
		}

		// Only check actors that are visible on screen
		if (!ProjectWorldLocationToScreen(TargetIterator->GetActorLocation(), Target.ScreenPosition) ||
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
		return (Target.TargetActor == LockonTarget);
	});

	FTarget CurrentTarget;

	if (FoundTarget == nullptr)
	{
		// Add current target to full list
		CurrentTarget.TargetActor = LockonTarget;
		CurrentTarget.Distance = PossessedCharacter->GetDistanceTo(LockonTarget);
		CurrentTarget.ScreenPosition = FVector2D::ZeroVector;

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
	if (LockonTarget != nullptr)
	{
		// Calculate pitch and yaw based on distance to target
		FVector LookVector = (LockonTarget->GetActorLocation() - PossessedCharacter->GetHeadPosition());

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
	if (PossessedCharacter == nullptr)
	{
		return;
	}
	
	FRotator MovementRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector MovementBasis = MovementRotator.RotateVector(FVector::RightVector);
	PossessedCharacter->AddMovementInput(MovementBasis * Scale);
}

void ATHPlayerController::Dodge()
{
	if (PossessedCharacter == nullptr)
	{
		return;
	}

	// Calculate dodge vector
	FVector DodgeVector = PossessedCharacter->GetLastMovementInputVector();

	// If velocity is small or zero, use backwards direction
	if (DodgeVector.SizeSquared() < (SMALL_NUMBER * SMALL_NUMBER))
	{
		// Rotate the backward vector by the controller look
		FRotator NewRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);		
		DodgeVector = NewRotation.RotateVector(FVector::BackwardVector);
	}
	else
	{
		// Normalize non-backwards dodge vector
		DodgeVector.Normalize();
	}

	if (PossessedCharacter->GetCanDodge())
	{
		PossessedCharacter->Dodge(DodgeVector);
	}
	else
	{
		// Record our input in the input buffer
		FBufferedInput NewInput;
		NewInput.ActionType = FBufferedInput::EInputAction::Dodge;
		NewInput.RecordedInputVector = DodgeVector;

		QueuePlayerInput(NewInput);
	}
}




// Actions

void ATHPlayerController::PrimaryAttack()
{
	if (PossessedCharacter == nullptr)
	{
		return;
	}

	if (PossessedCharacter->GetCanAttack())
	{
		PossessedCharacter->PrimaryAttack();
	}
	else
	{
		FBufferedInput NewInput;
		NewInput.ActionType = FBufferedInput::EInputAction::PrimaryAttack;
		
		QueuePlayerInput(NewInput);
	}
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


void ATHPlayerController::SetTarget(AActor* NewTarget)
{
	LockonTarget = NewTarget;
	
	if (LockonTarget == nullptr)
	{
		// Detach the target indicator and hide it
		TargetIndicatorActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetIndicatorActor->SetActorHiddenInGame(true);
		return;
	}

	TargetIndicatorActor->AttachToActor(LockonTarget, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TargetIndicatorActor->SetActorHiddenInGame(false);
}




// Camera effects

void ATHPlayerController::ApplyHitShake(FVector Direction, float Amplitude)
{
	if (CustomCameraManager == nullptr || CustomCameraManager->GetHitCameraShake() == nullptr)
	{
		return;
	}

	// Apply our hit shake curve to the actual hit shake object
	if (CustomCameraManager->GetHitCameraShake()->ShakeCurve != HitShakeCurve)
	{
		CustomCameraManager->GetHitCameraShake()->ShakeCurve = HitShakeCurve;
	}

	CustomCameraManager->ApplyHitShake(Direction, Amplitude);
}





// Helper functions

void ATHPlayerController::QueuePlayerInput(FBufferedInput NewInput)
{
	NewInput.BufferedTime = GetWorld()->GetRealTimeSeconds();

	if (InputBuffer.IsFull())
	{
		InputBuffer.Dequeue();
	}

	InputBuffer.Enqueue(NewInput);
}

bool ATHPlayerController::TryConsumePlayerInput(const FBufferedInput* ConsumedInput)
{
	switch (ConsumedInput->ActionType)
	{
	case FBufferedInput::EInputAction::Dodge:
		if (PossessedCharacter->GetCanDodge())
		{
			PossessedCharacter->Dodge(ConsumedInput->RecordedInputVector);
			return true;
		}
		return false;
	
	case FBufferedInput::EInputAction::PrimaryAttack:
		if (PossessedCharacter->GetCanAttack())
		{
			PossessedCharacter->PrimaryAttack();
			return true;
		}
		return false;

	default:
		return false;
	}
}





// Struct operators

bool ATHPlayerController::FTarget::operator==(const FTarget& OtherTarget) const
{
	return TargetActor == OtherTarget.TargetActor;
}



