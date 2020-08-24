// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "Teams.h"



UTeam::UTeam()
{
    
}

bool UTeam::GetCanTargetTeam(TSubclassOf<UTeam> OtherTeam) const
{
    return TargetableTeams.Contains(OtherTeam);
}


bool UTeam::GetCanDamageTeam(TSubclassOf<UTeam> OtherTeam) const
{
    return DamageableTeams.Contains(OtherTeam);
}



// Team member default behavior

bool ICombatant::GetCanBeTargeted() const
{
    return true;
}

bool ICombatant::GetCanBeDamaged() const
{
    return true;
}

bool ICombatant::GetCanBeDamagedBy(TSubclassOf<UTeam> OtherTeam) const
{
    // Safety check in case this actor's team is invalid or master override
    // is set
    if (!GetCanBeDamaged() || GetTeam() == nullptr)
    {
        return false;
    }
    
    return GetTeam()->GetDefaultObject<UTeam>()->GetCanDamageTeam(OtherTeam);
}

bool ICombatant::GetCanBeTargetedBy(TSubclassOf<UTeam> OtherTeam) const
{
    // Safety check in case this actor's team is invalid or master override
    // is set
    if (!GetCanBeTargeted() || GetTeam() == nullptr)
    {
        return false;
    }
    
    return GetTeam()->GetDefaultObject<UTeam>()->GetCanTargetTeam(OtherTeam);
}
