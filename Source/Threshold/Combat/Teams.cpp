// Copyright � 2020 Spencer Melnick

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




bool ITeamMember::GetCanBeDamagedBy(TSubclassOf<UTeam> OtherTeam) const
{
    // Safety check in case this actor's team is invalid
    if (GetTeam() == nullptr)
    {
        return false;
    }
    
    return GetTeam()->GetDefaultObject<UTeam>()->GetCanDamageTeam(OtherTeam);
}

bool ITeamMember::GetCanBeTargetedBy(TSubclassOf<UTeam> OtherTeam) const
{
    // Safety check in case this actor's team is invalid
    if (GetTeam() == nullptr)
    {
        return false;
    }
    
    return GetTeam()->GetDefaultObject<UTeam>()->GetCanTargetTeam(OtherTeam);
}
