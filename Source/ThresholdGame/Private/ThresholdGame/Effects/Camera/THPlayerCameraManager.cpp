// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Effects/Camera/THPlayerCameraManager.h"

#include "ThresholdGame/Effects/Camera/HitShake_CameraModifier.h"


ATHPlayerCameraManager::ATHPlayerCameraManager()
    : Super()
{
    // Add our HitCameraShake class to the default modifiers
    DefaultModifiers.Add(UHitShake_CameraModifier::StaticClass());
}


void ATHPlayerCameraManager::PostInitializeComponents()
{
    // Super::PostInitializeComponents() will already create any default
    // modifiers
    Super::PostInitializeComponents();

    // Try to find and cache the HitCameraShake modifier
    HitShakeModifier = Cast<UHitShake_CameraModifier>(*ModifierList.FindByPredicate([](UCameraModifier* Modifier){
        return Modifier->GetClass() == UHitShake_CameraModifier::StaticClass(); }));
}

void ATHPlayerCameraManager::ApplyHitShake(FVector Direction, float Amplitude,
    float ShakeDuration, UCurveFloat* ShakeCurve)
{
    if (HitShakeModifier == nullptr)
    {
        return;
    }

    HitShakeModifier->ApplyHitShake(Direction, Amplitude, ShakeDuration, ShakeCurve);
}


UHitShake_CameraModifier* ATHPlayerCameraManager::GetHitShakeModifier() const
{
    return HitShakeModifier;
}


