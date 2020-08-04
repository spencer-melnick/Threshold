// Copyright � 2020 Spencer Melnick

#include "HitCameraShake.h"

#include "Threshold/Global/THGameInstance.h"
#include "Threshold/Global/THConfig.h"

UHitCameraShake::UHitCameraShake()
    : Super()
{
    
}

void UHitCameraShake::ApplyHitShake(FVector InDirection, float InAmplitude)
{
    ShakeDirection = InDirection.GetSafeNormal();
    ShakeAmplitude = InAmplitude;

    // Reset shake time and start playing a new shake
    bIsPlayingShake = true;
    CurrentShakeTime = 0.f;
}

void UHitCameraShake::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
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

