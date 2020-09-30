// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"


// Forward declarations

struct FItemDataBase;


/*
 * Class used as the base for any inventory item type
 */
UCLASS(Abstract, BlueprintType)
class INVENTORYSYSTEM_API UItemTypeBase : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Called when replicating an ItemType if it is not supported for networking (IsSupportedForNetworking() returns false).
	 * Functions similarly to standard NetSerialize on structs.
	 */
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) { bOutSuccess = true; return true; }

	/**
	 * Used to get the type of item data used for this item type
	 * @return Static struct of the item data or null if no data is required
	 */
	virtual UScriptStruct* GetItemDataType() const { return nullptr; }
	
	/**
	 * Allocates new item data specific to this item type
	 * @return Pointer to new item data
	 */
	virtual TSharedPtr<FItemDataBase> CreateItemData() const { return TSharedPtr<FItemDataBase>(); }

	/**
	 * Used to get the name of this item as it should appear in the inventory UI
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual FText GetItemName(TWeakPtr<FItemDataBase> ItemData) const { unimplemented(); return FText(); }

	/**
	 * Used to get the description of this item as it should appear in the inventory UI
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual FText GetItemDescription(TWeakPtr<FItemDataBase> ItemData) const { unimplemented(); return FText(); }

	/**
	 * Used to get an actor that can be rendered as a 3D display for this inventory item
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual TSoftClassPtr<AActor> GetPreviewActorClass(TWeakPtr<FItemDataBase> ItemData) const { unimplemented(); return TSoftClassPtr<AActor>(); }
};
