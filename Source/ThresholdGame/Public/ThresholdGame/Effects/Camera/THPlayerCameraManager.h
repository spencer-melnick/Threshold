// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "THPlayerCameraManager.generated.h"




UCLASS()
class THRESHOLDGAME_API ATHPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    ATHPlayerCameraManager();
    
    virtual void PostInitializeComponents() override;
    
    void ApplyHitShake(FVector Direction, float Amplitude, float ShakeDuration, class UCurveFloat* ShakeCurve);
    
    class UHitShake_CameraModifier* GetHitShakeModifier() const;

private:
    class UHitShake_CameraModifier* HitShakeModifier = nullptr;
};
