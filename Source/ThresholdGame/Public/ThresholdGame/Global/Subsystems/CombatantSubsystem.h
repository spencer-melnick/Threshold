// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/WeakInterfacePtr.h"
#include "CombatantSubsystem.generated.h"


// Forward declarations

class ICombatant;



/**
 * Simple subsystem to track combatants
 */
UCLASS()
class THRESHOLDGAME_API UCombatantSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void RegisterCombatant(ICombatant* NewCombatant);
    void UnregisterCombatant(ICombatant* RemovedCombatant);
    const TArray<TWeakInterfacePtr<ICombatant>>& GetCombatants() const
    {
	    return Combatants;
    };

private:
    TArray<TWeakInterfacePtr<ICombatant>> Combatants;
};
