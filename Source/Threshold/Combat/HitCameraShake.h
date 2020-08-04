// Copyright � 2020 Spencer Melnick

#pragma once


#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "HitCameraShake.generated.h"



UCLASS()
class UHitCameraShake : public UCameraModifier
{
    GENERATED_BODY()

public:
    UHitCameraShake();

    void ApplyHitShake(FVector InDirection, float InAmplitude);
    
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
        float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;


    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ShakeDuration = 0.2f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class UCurveFloat* ShakeCurve = nullptr;


private:
    FVector ShakeDirection;
    float ShakeAmplitude;
    bool bIsPlayingShake = false;
    float CurrentShakeTime = 0.f;
};
