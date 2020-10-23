// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Effects/Camera/HitShake_CameraModifier.h"
#include "ThresholdGame/Global/THGameInstance.h"
#include "ThresholdGame/Global/THConfig.h"

UHitShake_CameraModifier::UHitShake_CameraModifier()
    : Super()
{
    
}

void UHitShake_CameraModifier::ApplyHitShake(FVector InDirection, float InAmplitude, float InDuration, UCurveFloat* InShakeCurve)
{
    ShakeDirection = InDirection.GetSafeNormal();
    ShakeAmplitude = InAmplitude;

    // Reset shake time and start playing a new shake
    bIsPlayingShake = true;
    CurrentShakeTime = 0.f;

    // Apply settings
    ShakeDuration = InDuration;
    ShakeCurve = InShakeCurve;
}

void UHitShake_CameraModifier::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
    float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
    // Skip if we're not playing, or we're missing a valid curve
    if (!bIsPlayingShake || ShakeCurve == nullptr)
    {
        return;
    }

    CurrentShakeTime += DeltaTime;

    // Stop the shake on timeout and don't affect the camera
    if (CurrentShakeTime >= ShakeDuration)
    {
        bIsPlayingShake = false;
        return;
    }

    // Make sure to apply alpha so this modifier can support enable/disable blending
    float ShakeScale = ShakeAmplitude * Alpha * ShakeCurve->GetFloatValue(CurrentShakeTime);

    // Try to apply global shake scale with checks on World and GameInstance
    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        UTHGameInstance* GameInstance = GetWorld()->GetGameInstance<UTHGameInstance>();
        if (GameInstance != nullptr)
        {
            ShakeScale *= GameInstance->GetTHConfig()->ScreenShakeScale;
        }
    }

    NewViewLocation = ViewLocation + ShakeDirection * ShakeScale;
}

