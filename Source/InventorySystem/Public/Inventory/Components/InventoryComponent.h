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
	 * Try to add an inventory item to this inventory.
	 * @return The count of the items added on success or 0 on failure
	 */
	int32 AddItem(const FInventoryItem& NewItem);


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
