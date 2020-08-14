// Copyright © 2020 Spencer Melnick

#include "CombatantSubsystem.h"

#include "Threshold/Combat/Teams.h"



void UCombatantSubsystem::RegisterCombatant(ICombatant* NewCombatant)
{
    Combatants.AddUnique(TScriptInterface<ICombatant>(Cast<UObject>(NewCombatant)));
}

void UCombatantSubsystem::UnregisterCombatant(ICombatant* RemovedCombatant)
{
    Combatants.RemoveAll([RemovedCombatant](TScriptInterface<ICombatant> Combatant)
    {
        return Combatant == RemovedCombatant;
    });
}

const TArray<TScriptInterface<ICombatant>>& UCombatantSubsystem::GetCombatants() const
{
    return Combatants;   
}
