// Copyright � 2020 Spencer Melnick

#include "THPlayerCameraManager.h"

#include "Threshold/Combat/HitCameraShake.h"


ATHPlayerCameraManager::ATHPlayerCameraManager()
    : Super()
{
    // Add our HitCameraShake class to the default modifiers
    DefaultModifiers.Add(UHitCameraShake::StaticClass());
}


void ATHPlayerCameraManager::PostInitializeComponents()
{
    // Super::PostInitializeComponents() will already create any default
    // modifiers
    Super::PostInitializeComponents();

    // Try to find and cache the HitCameraShake modifier
    HitCameraShake = Cast<UHitCameraShake>(*ModifierList.FindByPredicate([](UCameraModifier* Modifier){
        return Modifier->GetClass() == UHitCameraShake::StaticClass(); }));
}

void ATHPlayerCameraManager::ApplyHitShake(FVector Direction, float Amplitude)
{
    if (HitCameraShake == nullptr)
    {
        return;
    }

    HitCameraShake->ApplyHitShake(Direction, Amplitude);
}


UHitCameraShake* ATHPlayerCameraManager::GetHitCameraShake() const
{
    return HitCameraShake;
}


