// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"


// Forward declarations

struct FInventoryItemDataBase;
class APreviewActor;


/*
 * Class used as the base for any inventory item type
 */
UCLASS(Abstract, BlueprintType)
class INVENTORYSYSTEM_API UInventoryItemTypeBase : public UObject
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
	virtual TSharedPtr<FInventoryItemDataBase> CreateItemData() const { return TSharedPtr<FInventoryItemDataBase>(); }

	/**
	 * Used to get the name of this item as it should appear in the inventory UI
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual FText GetItemName(TWeakPtr<FInventoryItemDataBase> ItemData) const { return FText(); }

	/**
	 * Used to get the description of this item as it should appear in the inventory UI
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual FText GetItemDescription(TWeakPtr<FInventoryItemDataBase> ItemData) const { return FText(); }

	/**
	 * Used to get an actor that can be rendered as a 3D display for this inventory item
	 * @param ItemData - Optional item data relevant to this item type
	 */
	virtual TSoftClassPtr<APreviewActor> GetPreviewActorClass(TWeakPtr<FInventoryItemDataBase> ItemData) const { return nullptr; }

	/**
	 * Overridden to determine inventory storage behavior
	 * @return Whether or not an inventory component should store multiple items with this item type
	 */
	virtual bool AllowsDuplicates() const { return true; }

	/**
	 * Overridden to determine inventory storage behavior
	 * @return Whether or not an inventory component should store multiple items with this item type
	 */
	virtual bool AllowsStacking() const { return false; }

	/**
	 * Try to add items to a stack of this type. Must be implemented for any type that allows stacking
	 * @param ItemData - Item data relevant to this item type. Should contain stack count. May be null, so you should
	 * add an assert for validity
	 * @param Count - Number of items of this type to try to add to the stack
	 * @return Number of items added to the stack. 0 on failure
	 */
	virtual int32 AddToStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const { unimplemented(); return 0; }

	/**
	 * Try to remove items from a stack of this type. Must be implemented for any type that allows stacking
	 * @param ItemData - Item data relevant to this item type. Should contain stack count. May be null, so you should
	 * add an assert for validity
	* @param Count - Number of items of this type to try to remove from stack
	 * @return Number of items removed from the stack. 0 on failure
	 */
	virtual int32 RemoveFromStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const { unimplemented(); return 0; }

	/**
	 * Try set the number of items in a stack of this type - does not need to check against max stack size
	 * @param ItemData - Item data relevant to this item type. Should contain stack count. May be null, so you should
	 * add an assert for validity
	 * @param Count - Number of items of this type try to set
	*/
	virtual void SetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const { unimplemented(); }

	/**
	 * Used to get the number of items in a stack of this type
	 * @param ItemData - Item data relevant to this item type. Should contain stack count. May be null, so you should
	 * add an assert for validity
	 * @return Number of items stored in the stack
	 */
	virtual int32 GetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData) const { unimplemented(); return 0; }

	/**
	 * Compare inventory item types, default implementation only checks for matching class
	 */
	virtual bool operator==(const UInventoryItemTypeBase& OtherType) { return GetClass() == OtherType.GetClass(); }

	bool operator!=(const UInventoryItemTypeBase& OtherType) { return !operator==(OtherType); }


	
protected:
	template <typename DataType>
	static TSharedPtr<DataType> ConvertDataChecked(TWeakPtr<FInventoryItemDataBase> ItemData)
	{
		static_assert(TIsDerivedFrom<DataType, FInventoryItemDataBase>::IsDerived, "Conversion data type must be derived from FInventoryItemDataBase");
		if (!ItemData.IsValid())
		{
			return nullptr;
		}

		const TSharedPtr<FInventoryItemDataBase> PinnedData = ItemData.Pin();
		if (PinnedData->GetScriptStruct() != DataType::StaticStruct())
		{
			return nullptr;
		}

		return StaticCastSharedPtr<DataType>(PinnedData);
	}
};
