// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"


// TODO: possibly move this interface code into another interface
// since it only has one function right now
UINTERFACE()
class UDamageable : public UInterface
{
    GENERATED_BODY()
};

class IDamageable
{
    GENERATED_BODY()

public:
    // Returns whether or not the actor is damageable. Should
    // not include team logic - acts as a master on/off switch
    virtual bool GetCanBeDamaged() const;
};
