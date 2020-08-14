// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatantSubsystem.generated.h"



UCLASS()
class UCombatantSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void RegisterCombatant(class ICombatant* NewCombatant);
    
    void UnregisterCombatant(class ICombatant* RemovedCombatant);

    const TArray<TScriptInterface<class ICombatant>>& GetCombatants() const;

private:
    UPROPERTY()
    TArray<TScriptInterface<class ICombatant>> Combatants;
};
