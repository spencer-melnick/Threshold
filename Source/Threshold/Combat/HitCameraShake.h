// Copyright � 2020 Spencer Melnick

#pragma once


#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "HitCameraShake.generated.h"


// TODO: Replace HitCameraShake with a CameraModifier
// (Camera modifiers can be applied to the player camera manager)

UCLASS()
class UHitCameraShake : public UCameraShake
{
    GENERATED_BODY()

public:
    UHitCameraShake();

    virtual void PlayShake(APlayerCameraManager* Camera, float Scale, ECameraAnimPlaySpace::Type InPlaySpace,
        FRotator UserPlaySpaceRot) override;

    virtual void UpdateAndApplyCameraShake(float DeltaTime, float Alpha, FMinimalViewInfo& InOutPOV) override;

    
    FVector ShakeDirection;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ShakeAmplitude;
};
