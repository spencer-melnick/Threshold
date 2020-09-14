// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"



/**
 * Struct that provides a simple handle for an inventory item. Stores information such as the stack size
 */
USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()

	FInventorySlot() = default;

	FInventorySlot(UClass* InClass, int32 InSize)
		: ItemClass(InClass), StackSize(InSize)
	{};

	UPROPERTY()
	UClass* ItemClass = nullptr;

	UPROPERTY()
	int32 StackSize = 0;


	
	// Inventory slot helpers
	
	// Add count number of items to the stack. Returns the actual number added, limited by the stack size
	int32 AddToStack(int32 Count, int32 MaxStackSize);
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
	int32 AddInventoryItem(UClass* ItemClass, int32 Count = 1);

	// Returns the first inventory slot matching this item class
	FInventorySlot* FindFirstItemSlot(UClass* ItemClass);

	// Returns all inventory slots matching this item class
	TArray<FInventorySlot*> FindAllItemSlots(UClass* ItemClass);

	// Removes the item from the inventory based on class. Returns the count of items removed from the inventory - will
	// be zero if no items were removed
	int32 RemoveInventoryItem(UClass* ItemClass, int32 Count = 1);
	

	
	// Editor properties

	// Maximum size of the inventory
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	int32 MaxInventorySize = 100;


protected:
	// Network replication

	UFUNCTION()
	void OnRep_Inventory();
	
	
	
	// Helper functions

	// Add an item with checks for uniqueness
	int32 AddUniqueItem(UClass* ItemClass);

	// Add an item with checks for inventory size
	int32 AddNewItem(UClass* ItemClass, int32 Count = 1, int32 MaxStackSize = 1);

	// Add an item with checks for existing stacks
	int32 AddStackItem(UClass* ItemClass, int32 Count, int32 MaxStackSize);

	// Add to stack with checks for uniqueness
	int32 AddUniqueStackItem(UClass* ItemClass, int32 Count, int32 MaxStackSize);
	

	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FInventorySlot> Inventory;
};
