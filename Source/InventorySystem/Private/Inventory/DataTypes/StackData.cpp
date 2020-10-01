// Copyright (c) 2020 Spencer Melnick

#include "Inventory/DataTypes/StackData.h"

bool FInventoryStackData::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	Ar << StackCount;
	bOutSuccess = true;
	return true;
}

