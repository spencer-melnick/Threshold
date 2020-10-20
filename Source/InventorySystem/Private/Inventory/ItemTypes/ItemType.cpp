// Copyright (c) 2020 Spencer Melnick

#include "Inventory/ItemTypes/ItemType.h"


// UInventoryItemType

bool UInventoryItemType::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	Ar << ItemName;
	Ar << ItemDescription;
	Ar << PreviewActorClass;
	Ar << bAllowsDuplicates;

	bOutSuccess = true;
	return true;
}

bool UInventoryItemType::operator==(const UInventoryItemTypeBase& OtherType) const
{
	if (!Super::operator==(OtherType))
	{
		return false;
	}

	// Base operator checks types for matching class so we should be able to cast the other type safely
	const UInventoryItemType& OtherTypeCasted = static_cast<const UInventoryItemType&>(OtherType);
	return operator==(OtherTypeCasted);
}

bool UInventoryItemType::operator==(const UInventoryItemType& OtherType) const
{
	return GetPrimaryAssetId() == OtherType.GetPrimaryAssetId();
}

