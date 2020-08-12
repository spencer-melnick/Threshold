// Copyright � 2020 Spencer Melnick

#include "Targetable.h"



FVector ITargetable::GetTargetWorldLocation() const
{
    return FVector::ZeroVector;
}

FVector ITargetable::GetTargetLocalLocation() const
{
    return FVector::ZeroVector;
}

bool ITargetable::GetCanBeTargeted() const
{
    return true;
}


