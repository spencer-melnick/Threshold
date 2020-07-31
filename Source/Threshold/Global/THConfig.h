// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "THConfig.generated.h"





UCLASS(Config = "Game")
class THRESHOLD_API UTHConfig : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(Config)
    float ScreenShakeScale = 1.f;
};
