// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"

#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


/**
 * Struct that wraps our array for fast net array serialization
 */
USTRUCT()
struct FFastInventoryArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FInventoryItemHandle> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryItemHandle, FFastInventoryArray>( Items, DeltaParams, *this );
	}
};



/**
 * Component that provides a simple interface for tracking inventory items and replicating them. Should only
 * be stored on the PlayerState (note that PlayerState may need an increased update frequency to replicate
 * the inventory more reliably)
 */
UCLASS()
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	
	// Engine overrides

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// Inventory accessors

	// Adds the item to the inventory. Returns the count of the items added - will be zero if the item cannot be added
	// due to storage behavior such as a unique item with duplicates already in the inventory, a full item stack, or a
	// full inventory
	int32 AddInventoryItem(FInventoryItem* Item);
	

	
	// Editor properties

	// Maximum size of the inventory
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	int32 MaxInventorySize = 100;


protected:
	// Network replication

	UFUNCTION()
	void OnRep_Inventory();
	

	

	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere)
	FFastInventoryArray InventoryArray;
};


// Enable fast array serialization

template<>
struct TStructOpsTypeTraits< FFastInventoryArray > : public TStructOpsTypeTraitsBase2< FFastInventoryArray >
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};
