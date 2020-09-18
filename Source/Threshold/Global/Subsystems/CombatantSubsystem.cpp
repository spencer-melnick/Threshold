﻿// Copyright (c) 2020 Spencer Melnick

#include "CombatantSubsystem.h"

#include "Threshold/Combat/Teams.h"



void UCombatantSubsystem::RegisterCombatant(ICombatant* NewCombatant)
{
    Combatants.AddUnique(TWeakInterfacePtr<ICombatant>(*NewCombatant));
}

void UCombatantSubsystem::UnregisterCombatant(ICombatant* RemovedCombatant)
{
    Combatants.RemoveAll([RemovedCombatant](const TWeakInterfacePtr<ICombatant> Combatant)
    {
        return &(*Combatant) == RemovedCombatant || !Combatant.IsValid();
    });
}
