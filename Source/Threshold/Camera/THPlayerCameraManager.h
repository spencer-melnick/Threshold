// Copyright � 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "THPlayerCameraManager.generated.h"




UCLASS()
class THRESHOLD_API ATHPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    ATHPlayerCameraManager();
    
    virtual void PostInitializeComponents() override;
    
    void ApplyHitShake(FVector Direction, float Amplitude);
    
    class UHitCameraShake* GetHitCameraShake() const;

private:
    class UHitCameraShake* HitCameraShake = nullptr;
};
