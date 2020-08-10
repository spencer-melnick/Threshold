// Copyright � 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Targetable.generated.h"



UINTERFACE()
class UTargetable : public UInterface
{
    GENERATED_BODY()
};

class ITargetable
{
    GENERATED_BODY()

public:
    // Returns the location where the target marker should be placed
    // in world space
    virtual FVector GetTargetWorldLocation() const;

    // Returns the location where the target marker should be placed
    // in actor space
    virtual FVector GetTargetLocalLocation() const;
};
