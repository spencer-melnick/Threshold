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

    // Returns whether or not the character can be targeted. Should
    // not include team logic - works as a master on/off
    virtual bool GetCanBeTargeted() const;

    // Attach the desired actor to the target location or socket. If
    // this returns false, the attachment was unsuccessful, and
    // the caller should try to handle attachment itself
    // TODO: possibly remove the default function definition
    // to force implementors to define this function theirselves
    // and simplify the external implementations
    virtual bool AttachToTarget(AActor* ActorToBeAttached);
};
