// Copyright © 2020 Spencer Melnick


#include "THCharacterMovement.h"


// Engine overrides

FVector UTHCharacterMovement::ConsumeInputVector()
{
	FVector InputVector = Super::ConsumeInputVector();

	if (MovementMode != MOVE_Custom)
	{
		return InputVector;
	}

	// On custom movement mode we can recompute the input vector here!
	switch (CustomMovementMode)
	{
	case ETHCustomMovementTypes::CUSTOMMOVE_Dodge:
		return DodgeVector;
	default:
		return FVector::ZeroVector;
	}
}





// Accessors

FVector UTHCharacterMovement::GetDodgeVector() const
{
	return DodgeVector;
}








// Engine overrides

void UTHCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case ETHCustomMovementTypes::CUSTOMMOVE_Dodge:
			// Compute the dodge vector

			if (Velocity.SizeSquared() < (SMALL_NUMBER * SMALL_NUMBER))
			{
				// If velocity is small, use backwards direction
				DodgeVector = -GetOwner()->GetActorForwardVector();
			}
			else
			{
				// Use direction of current movement
				DodgeVector = Velocity.GetSafeNormal();
			}
			break;
		default:
			break;
		}
	}
}

void UTHCharacterMovement::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case ETHCustomMovementTypes::CUSTOMMOVE_Dodge:
		// For now dodge movement is just a type of walking movement
		PhysWalking(DeltaTime, Iterations);
		break;
	default:
		break;
	}
}

float UTHCharacterMovement::GetMaxAcceleration() const
{
	// If not using a custom movement mode, return the regular max acceleration
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxAcceleration();
	}

	switch (CustomMovementMode)
	{
	case ETHCustomMovementTypes::CUSTOMMOVE_Dodge:
		return DodgeAcceleration;
	default:
		return 0.f;
	}
}


float UTHCharacterMovement::GetMaxSpeed() const
{
	// If not using a custom movement mode, return the regular max speed
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxSpeed();
	}

	switch (CustomMovementMode)
	{
	case ETHCustomMovementTypes::CUSTOMMOVE_Dodge:
		return DodgeSpeed;
	default:
		return 0.f;
	}
}




