// Copyright (c) 2020 Spencer Melnick


#include "ThresholdGame/Character/Movement/THCharacterMovement.h"



FRotator UTHCharacterMovement::GetDeltaRotation(float DeltaTime) const
{
	if (!bShouldRotateInAir && IsFalling())
	{
		// If we're falling and can't rotate in the air, don't do anything
		return FRotator::ZeroRotator;
	}
	
	const FRotator OriginalDeltaRotation = Super::GetDeltaRotation(DeltaTime);

	// If rotation isn't proportional to speed, just use the original delta rotation
	if (!bRotationProportionalToSpeed)
	{
		return OriginalDeltaRotation;
	}

	// We can probably use the last update velocity since it will only be one tick off
	const float ScaledSpeed = GetLastUpdateVelocity().Size() / GetMaxSpeed();
	return OriginalDeltaRotation * ScaledSpeed;
}

