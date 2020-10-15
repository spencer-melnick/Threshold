// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "InventoryComponent.generated.h"


/**
 * Array used to support fast serialization of inventory items
 */
USTRUCT()
struct FInventoryArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FInventoryItem> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize(Items, DeltaParams, *this);
	}
};



// Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedDelegate);



/**
 * Component used to hold an inventory filed with an array of inventory items
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UInventoryComponent();


	// Engine overrides

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Inventory access

	/**
	 * Try to add an inventory item to this inventory via a copy.
	 * @return The count of the items added on success or 0 on failure
	 */
	int32 AddItem(const FInventoryItem& NewItem);

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
	 * @return An array of inventory item pointers. NOTE: these are not guaranteed to be valid for very long, since any
	 * insertions or deletions on the underlying array could invalidate the pointers.
	 */
	TArray<FInventoryItem*> GetAllItemsByType(UInventoryItemTypeBase* ItemType);

	/**
	 * Find the first inventory item that matches a type
	 * @param ItemType - Pointer to the type object to compare against
	 * @return Pointer to the first element that matches the type or nullptr if there are no matches. NOTE: this pointer
	 * is not guaranteed to be valid for very long, since any insertions or deletions on the underlying array could
	 * invalidate the pointers.
	 */
	FInventoryItem* GetFirstItemByType(UInventoryItemTypeBase* ItemType);

	/**
	 * Access the underlying array object
	 */
	const TArray<FInventoryItem>& GetArray() const { return InventoryArray.Items; }


	// Delegates

	// Called whenever the array changes
	UPROPERTY(BlueprintAssignable)
	FInventoryChangedDelegate InventoryChangedDelegate;


protected:
	UFUNCTION()
	virtual void OnRep_InventoryArray();


private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_InventoryArray)
	FInventoryArray InventoryArray;
};



/**
* Enables fast network serialization of an inventory array
*/
template <>
struct TStructOpsTypeTraits<FInventoryArray> : public TStructOpsTypeTraitsBase2<FInventoryArray>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};
