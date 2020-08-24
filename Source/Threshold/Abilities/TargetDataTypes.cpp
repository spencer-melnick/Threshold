// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "TargetDataTypes.h"

bool FAbilityDirectionalData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Direction.NetSerialize(Ar, Map, bOutSuccess);

	bOutSuccess = true;
	return true;
}

