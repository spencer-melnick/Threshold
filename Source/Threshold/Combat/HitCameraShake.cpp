// Copyright � 2020 Spencer Melnick

#include "HitCameraShake.h"

UHitCameraShake::UHitCameraShake()
    : Super()
{
    // Overwrite normal camera shake parameters
    PlaySpace = ECameraAnimPlaySpace::World;
	bSingleInstance = true;
}




// Below function is a modification of original Unreal Engine source code
// Original code is
// Copyright Epic Games, Inc. All Rights Reserved.

void UHitCameraShake::PlayShake(APlayerCameraManager* Camera, float Scale,
    ECameraAnimPlaySpace::Type InPlaySpace, FRotator UserPlaySpaceRot)
{
    ShakeScale = Scale;
    CameraOwner = Camera;

    // init oscillations
    if (OscillationDuration != 0.f)
    {
        if (OscillatorTimeRemaining > 0.f)
        {
            // this shake was already playing
            OscillatorTimeRemaining = OscillationDuration;

            if (bBlendingOut)
            {
                bBlendingOut = false;
                CurrentBlendOutTime = 0.f;

                // stop any blendout and reverse it to a blendin
                if (OscillationBlendInTime > 0.f)
                {
                    bBlendingIn = true;
                    CurrentBlendInTime = OscillationBlendInTime * (1.f - CurrentBlendOutTime / OscillationBlendOutTime);
                }
                else
                {
                    bBlendingIn = false;
                    CurrentBlendInTime = 0.f;
                }
            }
        }
        else
        {
            if (Anim != nullptr)
            {
                UE_LOG(LogTemp, Warning, TEXT("UHitCameraShake %s has an animation, but the animation will never play"),
                    *GetNameSafe(this))
            }

        	// Use zero offsets as default
            RotSinOffset = FVector::ZeroVector;
            LocSinOffset = FVector::ZeroVector;
            FOVSinOffset = 0.f;

            InitialLocSinOffset = LocSinOffset;
            InitialRotSinOffset = RotSinOffset;
            InitialFOVSinOffset = FOVSinOffset;

            OscillatorTimeRemaining = OscillationDuration;

            if (OscillationBlendInTime > 0.f)
            {
                bBlendingIn = true;
                CurrentBlendInTime = 0.f;
            }
        }
    }

    PlaySpace = InPlaySpace;
    if (InPlaySpace == ECameraAnimPlaySpace::UserDefined)
    {
        UserPlaySpaceMatrix = FRotationMatrix(UserPlaySpaceRot);
    }

    ReceivePlayShake(Scale);
}




// Below function is a modification of original Unreal Engine source code
// Original code is
// Copyright Epic Games, Inc. All Rights Reserved.

void UHitCameraShake::UpdateAndApplyCameraShake(float DeltaTime, float Alpha, FMinimalViewInfo& InOutPOV)
{
	// this is the base scale for the whole shake, anim and oscillation alike
	float const BaseShakeScale = FMath::Max<float>(Alpha * ShakeScale, 0.0f);

	// update oscillation times
	if (OscillatorTimeRemaining > 0.f)
	{
		OscillatorTimeRemaining -= DeltaTime;
		OscillatorTimeRemaining = FMath::Max(0.f, OscillatorTimeRemaining);
	}
	if (bBlendingIn)
	{
		CurrentBlendInTime += DeltaTime;
	}
	if (bBlendingOut)
	{
		CurrentBlendOutTime += DeltaTime;
	}

	// see if we've crossed any important time thresholds and deal appropriately
	bool bOscillationFinished = false;

	if (OscillatorTimeRemaining == 0.f)
	{
		// finished!
		bOscillationFinished = true;
	}
	else if (OscillatorTimeRemaining < 0.0f)
	{
		// indefinite shaking
	}
	else if (OscillatorTimeRemaining < OscillationBlendOutTime)
	{
		// start blending out
		bBlendingOut = true;
		CurrentBlendOutTime = OscillationBlendOutTime - OscillatorTimeRemaining;
	}

	if (bBlendingIn)
	{
		if (CurrentBlendInTime > OscillationBlendInTime)
		{
			// done blending in!
			bBlendingIn = false;
		}
	}
	if (bBlendingOut)
	{
		if (CurrentBlendOutTime > OscillationBlendOutTime)
		{
			// done!!
			CurrentBlendOutTime = OscillationBlendOutTime;
			bOscillationFinished = true;
		}
	}

	// Do not update oscillation further if finished
	if (bOscillationFinished == false)
	{
		// calculate blend weight. calculating separately and taking the minimum handles overlapping blends nicely.
		float const BlendInWeight = (bBlendingIn) ? (CurrentBlendInTime / OscillationBlendInTime) : 1.f;
		float const BlendOutWeight = (bBlendingOut) ? (1.f - CurrentBlendOutTime / OscillationBlendOutTime) : 1.f;
		float const CurrentBlendWeight = FMath::Min(BlendInWeight, BlendOutWeight);

		// this is the oscillation scale, which includes oscillation fading
		float const OscillationScale = BaseShakeScale * CurrentBlendWeight;

		if (OscillationScale > 0.f)
		{
			// View location offset, compute sin wave value for each component
			FVector	LocOffset = ShakeDirection * ShakeAmplitude;
			LocOffset *= OscillationScale;

			FRotator RotOffset;

			if (PlaySpace == ECameraAnimPlaySpace::CameraLocal)
			{
				// the else case will handle this as well, but this is the faster, cleaner, most common code path

				// apply loc offset relative to camera orientation
				FRotationMatrix CamRotMatrix(InOutPOV.Rotation);
				InOutPOV.Location += CamRotMatrix.TransformVector(LocOffset);

				// apply rot offset relative to camera orientation
				FRotationMatrix const AnimRotMat(RotOffset);
				InOutPOV.Rotation = (AnimRotMat * FRotationMatrix(InOutPOV.Rotation)).Rotator();
			}
			else
			{
				// find desired space
				FMatrix const PlaySpaceToWorld = (PlaySpace == ECameraAnimPlaySpace::UserDefined) ? UserPlaySpaceMatrix : FMatrix::Identity;

				// apply loc offset relative to desired space
				InOutPOV.Location += PlaySpaceToWorld.TransformVector(LocOffset);

				// apply rot offset relative to desired space

				// find transform from camera to the "play space"
				FRotationMatrix const CamToWorld(InOutPOV.Rotation);
				FMatrix const CameraToPlaySpace = CamToWorld * PlaySpaceToWorld.Inverse();			// CameraToWorld * WorldToPlaySpace

				// find transform from anim (applied in playspace) back to camera
				FRotationMatrix const AnimToPlaySpace(RotOffset);
				FMatrix const AnimToCamera = AnimToPlaySpace * CameraToPlaySpace.Inverse();			// AnimToPlaySpace * PlaySpaceToCamera

				// RCS = rotated camera space, meaning camera space after it's been animated
				// this is what we're looking for, the diff between rotated cam space and regular cam space.
				// apply the transform back to camera space from the post-animated transform to get the RCS
				FMatrix const RCSToCamera = CameraToPlaySpace * AnimToCamera;

				// now apply to real camera
				InOutPOV.Rotation = (RCSToCamera * CamToWorld).Rotator();
			}

			// Compute FOV change
			InOutPOV.FOV += OscillationScale * FFOscillator::UpdateOffset(FOVOscillation, FOVSinOffset, DeltaTime);
		}
	}

	BlueprintUpdateCameraShake(DeltaTime, Alpha, InOutPOV, InOutPOV);
}


