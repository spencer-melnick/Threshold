// Copyright (c) 2020 Spencer Melnick

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItem.h"
#include "Threshold/Threshold.h"


// FInventorySlot

int32 FInventorySlot::AddToStack(int32 Count, int32 MaxStackSize)
{
	// Limit our stack amount by the max stack size
	const int32 StartingStackSize = StackSize;
	StackSize = FMath::Min(StackSize + Count, MaxStackSize);

	// Return the amount we actually added
	return StackSize - StartingStackSize;
}




// UInventoryComponent

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}



// Engine overrides

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, MaxInventorySize)
	DOREPLIFETIME(UInventoryComponent, Inventory)
}



// Inventory accessors

int32 UInventoryComponent::AddInventoryItem(UClass* ItemClass, int32 Count)
{
	if (!ItemClass || !ItemClass->ImplementsInterface(UInventoryItem::StaticClass()))
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot add %s to %s - not a valid InventoryItem"),
			*GetNameSafe(ItemClass), *GetNameSafe(this))
		return 0;
	}

	const UObject* ItemObject = ItemClass->GetDefaultObject(true);
	const IInventoryItem* InventoryItem = Cast<IInventoryItem>(ItemObject);
	check(ItemObject && InventoryItem);

	switch (InventoryItem->GetStorageBehavior())
	{
		case EInventoryStorageBehavior::Unique:
			return AddUniqueItem(ItemClass);

		case EInventoryStorageBehavior::Duplicate:
			return AddNewItem(ItemClass);

		case EInventoryStorageBehavior::Stack:
			return AddStackItem(ItemClass, Count, InventoryItem->GetMaxStackSize());
		
		case EInventoryStorageBehavior::StackUnique:
			return AddUniqueStackItem(ItemClass, Count, InventoryItem->GetMaxStackSize());
	}

	return 0;
}

FInventorySlot* UInventoryComponent::FindFirstItemSlot(UClass* ItemClass)
{
	return Inventory.FindByPredicate([ItemClass](const FInventorySlot& InventorySlot) -> bool
	{
		return InventorySlot.ItemClass == ItemClass;
	});
}

TArray<FInventorySlot*> UInventoryComponent::FindAllItemSlots(UClass* ItemClass)
{
	TArray<FInventorySlot*> FoundSlots;

	for (FInventorySlot& InventorySlot : Inventory)
	{
		if (InventorySlot.ItemClass == ItemClass)
		{
			FoundSlots.Add(&InventorySlot);
		}
	}

	return FoundSlots;
}

int32 UInventoryComponent::RemoveInventoryItem(UClass* ItemClass, int32 Count)
{
	int32 RemovedCount = 0;
	const UObject* ItemObject = ItemClass->GetDefaultObject(true);
	const IInventoryItem* InventoryItem = Cast<IInventoryItem>(ItemObject);
	check(ItemObject && InventoryItem);

	if (Count <= 1)
	{
		// If we're only removing one of an item just get the first slot
		FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemClass);

		if (!ExistingSlot)
		{
			// If there are no slots, we can't remove anything
			return 0;
		}

		ExistingSlot->StackSize--;
		RemovedCount = Count;
	}
	else
	{
		// If we're removing multiples, we need all slots with the item
		TArray<FInventorySlot*> ExistingSlots = FindAllItemSlots(ItemClass);

		for (FInventorySlot* ExistingSlot : ExistingSlots)
		{
			// Limit removal by the number of items currently in the stack
			const int32 RemainingRemovalCount = Count - RemovedCount; 
			const int32 NextRemovalCount = FMath::Min(ExistingSlot->StackSize, RemainingRemovalCount);

			// Remove from the stack and see how many are left to remove
			ExistingSlot->StackSize -= NextRemovalCount;
			RemovedCount += NextRemovalCount;

			if (RemovedCount >= Count)
			{
				// If we've already removed count elements we're done
				break;
			}
		}
	}

	// Remove all items with 0 stack size
	Inventory.RemoveAll([](const FInventorySlot& InventorySlot) -> bool
	{
		return InventorySlot.StackSize <= 0;
	});

	return RemovedCount;
}





// Network replication

void UInventoryComponent::OnRep_Inventory()
{
	// Verify that all inventory items are valid objects

	bool bFoundBadSlot = false;

	for (FInventorySlot& InventorySlot : Inventory)
	{
		if (!InventorySlot.ItemClass || InventorySlot.ItemClass->ImplementsInterface(UInventoryItem::StaticClass()))
		{
			bFoundBadSlot = true;
			break;
		}
	}

	if (bFoundBadSlot)
	{
		UE_LOG(LogThresholdGeneral, Error, TEXT("Received bad item class on inventory replication"))
		// TODO: Exit here!
	}
}




// Helper functions

int32 UInventoryComponent::AddUniqueItem(UClass* ItemClass)
{
	FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemClass);

	if (ExistingSlot)
	{
		// We can't add the item if it's already in the inventory
		return 0;
	}

	// We can drop into duplicate item behavior if there is no existing item
	return AddNewItem(ItemClass);
}

int32 UInventoryComponent::AddNewItem(UClass* ItemClass, int32 Count, int32 MaxStackSize)
{	
	if (Inventory.Num() >= MaxInventorySize)
	{
		// We can't add the item if the inventory is full
		return 0;
	}

	int32 CountAdded = Count;

	if (Count > 1)
	{
		// Limit new item count by the max stack size
		CountAdded = FMath::Min(MaxStackSize, Count);
	}

	// Add the item in a new slot
	Inventory.Emplace(ItemClass, CountAdded);
	return CountAdded;
}

int32 UInventoryComponent::AddStackItem(UClass* ItemClass, int32 Count, int32 MaxStackSize)
{
	TArray<FInventorySlot*> ExistingSlots = FindAllItemSlots(ItemClass);
	int32 TotalCountAdded = 0;

	for (FInventorySlot* ExistingSlot : ExistingSlots)
	{
		// For each existing stack, try to add to the stack
		TotalCountAdded += ExistingSlot->AddToStack(Count, MaxStackSize);
		
		if (TotalCountAdded == Count)
		{
			// If we added everything, we're done
			return TotalCountAdded;
		}
	}

	while (TotalCountAdded < Count)
	{
		// Keep adding new stacks until we've added the entire count
		const int32 NewStackCount = AddNewItem(ItemClass, Count - TotalCountAdded, MaxStackSize);
		
		if (NewStackCount == 0)
		{
			// If the new stack count is 0, then the inventory is full and we can't add any new stacks
			break;
		}

		// Keep incrementing our added count by each new stack size
		TotalCountAdded += NewStackCount;
	}

	return TotalCountAdded;
}

int32 UInventoryComponent::AddUniqueStackItem(UClass* ItemClass, int32 Count, int32 MaxStackSize)
{
	FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemClass);

	if (ExistingSlot)
	{
		// We can't create another stack, so we can only add as much as the existing stack can hold
		return ExistingSlot->AddToStack(Count, MaxStackSize);
	}

	// Add a new stack slot and return it's size
	const int32 NewStackSize = FMath::Min(Count, MaxStackSize);
	Inventory.Emplace(ItemClass, NewStackSize);
	return NewStackSize;
}

