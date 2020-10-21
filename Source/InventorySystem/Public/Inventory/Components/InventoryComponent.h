// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryArray.h"
#include "InventoryComponent.generated.h"



// Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedDelegate);



/**
 * Component used to hold an inventory filed with an array of inventory items
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend struct FInventoryArrayHandle;
	
public:
	UInventoryComponent();


	// Type aliases

	using FAdditionResult = TPair<int32, FInventoryArrayHandle>;


	// Engine overrides

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Inventory access

	/**
	 * Try to add an inventory item to this inventory via a copy.
	 * @return The count of the items added on success or 0 on failure, and a handle to the new item (if any)
	 */
	TPair<int32, FInventoryArrayHandle> AddItem(const FInventoryItem& NewItem);

	/**
	 * Try to remove an inventory item from this inventory by type
	 * @param ItemType - The type of the item to remove
	 * @param Count - The number of items to be removed
	 * @return The count of the items removed or 0 if none were removed
	 */
	int32 RemoveItem(UInventoryItemTypeBase* ItemType, int32 Count);

	/**
	 * Find all inventory items that match a type
	 * @param ItemType - Pointer to the type object to compare against
	 * @return An array of temporary inventory item pointers
	 */
	TArray<FInventoryItem*> GetAllItemsByTypeTemporary(UInventoryItemTypeBase* ItemType);

	/**
	 * Find all inventory items that match a type
	 * @param ItemType - Pointer to the type object to compare against
	 * @return An array of inventory item handles.
	 */
	TArray<FInventoryArrayHandle> GetAllItemsByType(UInventoryItemTypeBase* ItemType);
	
	/**
	 * Find the first inventory item that matches a type
	 * @param ItemType - Pointer to the type object to compare against
	 * @return Temporary pointer to the first item that matches the type, or nullptr if there is none
	 */
	FInventoryItem* GetFirstItemByTypeTemporary(UInventoryItemTypeBase* ItemType);

	/**
	 * Find the first inventory item that matches a type
	 * @param ItemType - Pointer to the type object to compare against
	 * @return Handle to the first item that matches the type (handle will be invalid if no item matches the type)
	 */
	FInventoryArrayHandle GetFirstItemByType(UInventoryItemTypeBase* ItemType);

	/**
	 * Access the underlying array object
	 */
	const TArray<FInventoryItem>& GetArray() const { return InventoryArray.GetArray(); }

	/**
	 * Returns handles to all of the items in the underlying array
	 */
	TArray<FInventoryArrayHandle> GetArrayHandles() { return InventoryArray.GetArrayHandles(); }


	// Delegates

	// Called whenever the array changes (either due to addition, deletion, or modification of an element)
	// Will be called whenever MarkItemDirty or MarkArrayDirty are called
	UPROPERTY(BlueprintAssignable)
	FInventoryChangedDelegate OnInventoryChanged;



protected:

	// Replication
	
	UFUNCTION()
	virtual void OnRep_InventoryArray();


	// Delegates

	void InventoryChanged();
	

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_InventoryArray)
	FInventoryArray InventoryArray;
};
