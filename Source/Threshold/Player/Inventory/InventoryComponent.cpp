// Copyright (c) 2020 Spencer Melnick

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItem.h"
#include "Kismet/GameplayStatics.h"
#include "Threshold/Threshold.h"
#include "Threshold/Global/Subsystems/InventorySubsystem.h"
#include "Threshold/Player/Inventory/Items/InventoryTableItem.h"


// FInventorySlot

int32 FInventorySlot::AddToStack(int32 Count, int32 MaxStackSize)
{
	// Limit our stack amount by the max stack size
	const int32 StartingStackSize = StackSize;
	StackSize = FMath::Min(StackSize + Count, MaxStackSize);

	// Return the amount we actually added
	return StackSize - StartingStackSize;
}

bool FInventorySlot::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bool bIncludeId = false;
	UClass* ItemClass = nullptr;

	if (Ar.IsSaving())
	{
		// Check for a valid object
		if (!ItemObject)
		{
			UE_LOG(LogThresholdGeneral, Error, TEXT("Cannot net serialize inventory slot with no valid object"))
			bOutSuccess = false;
			return false;
		}

		// Get our item class
		ItemClass = ItemObject.GetObject()->GetClass();

		if (ItemClass == UInventoryTableItem::StaticClass())
		{
			// Generate the hash of our item name
			bIncludeId = true;
			ItemId = UInventorySubsystem::HashName(ItemObject.GetObject()->GetFName());
		}
	}

	// (De)serialize the stack size and item class
	Ar << StackSize;
	Ar << ItemClass;

	if (Ar.IsLoading())
	{
		if (!ItemClass || !ItemClass->ImplementsInterface(UInventoryItem::StaticClass()))
		{
			UE_LOG(LogThresholdGeneral, Error, TEXT("Invalid object class when net deserializing inventory slot"))
			bOutSuccess = false;
			return false;
		}

		// Create the item object from the CDO
		ItemObject = TScriptInterface<IInventoryItem>(ItemClass->GetDefaultObject());

		if (ItemClass == UInventoryTableItem::StaticClass())
		{
			// If the item class is a table item, we should also receive the ID
			bIncludeId = true;
		}
	}

	if (bIncludeId)
	{
		// If we generated or are expecting an item ID, (de)serialize it here
		Ar << ItemId;
	}

	bOutSuccess = true;
	return true;
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

int32 UInventoryComponent::AddInventoryItem(TScriptInterface<IInventoryItem> ItemObject, int32 Count)
{
	if (!ItemObject)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot add %s to %s - not a valid InventoryItem"),
			*GetNameSafe(ItemObject.GetObject()), *GetNameSafe(this))
		return 0;
	}

	switch (ItemObject->GetStorageBehavior())
	{
		case EInventoryStorageBehavior::Unique:
			return AddUniqueItem(ItemObject);

		case EInventoryStorageBehavior::Duplicate:
			return AddNewItem(ItemObject);

		case EInventoryStorageBehavior::Stack:
			return AddStackItem(ItemObject, Count, ItemObject->GetMaxStackSize());
		
		case EInventoryStorageBehavior::StackUnique:
			return AddUniqueStackItem(ItemObject, Count, ItemObject->GetMaxStackSize());
	}

	return 0;
}

FInventorySlot* UInventoryComponent::FindFirstItemSlot(TScriptInterface<IInventoryItem> ItemObject)
{
	return Inventory.FindByPredicate([ItemObject](const FInventorySlot& InventorySlot) -> bool
	{
		return InventorySlot.ItemObject == ItemObject;
	});
}

TArray<FInventorySlot*> UInventoryComponent::FindAllItemSlots(TScriptInterface<IInventoryItem> ItemObject)
{
	TArray<FInventorySlot*> FoundSlots;

	for (FInventorySlot& InventorySlot : Inventory)
	{
		if (InventorySlot.ItemObject == ItemObject)
		{
			FoundSlots.Add(&InventorySlot);
		}
	}

	return FoundSlots;
}

int32 UInventoryComponent::RemoveInventoryItem(TScriptInterface<IInventoryItem> ItemObject, int32 Count)
{
	int32 RemovedCount = 0;

	if (Count <= 1)
	{
		// If we're only removing one of an item just get the first slot
		FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemObject);

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
		TArray<FInventorySlot*> ExistingSlots = FindAllItemSlots(ItemObject);

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
	for (FInventorySlot& InventorySlot : Inventory)
	{
		if (!InventorySlot.ItemObject)
		{
			continue;
		}

		if (InventorySlot.ItemObject.GetObject()->GetClass() == UInventoryTableItem::StaticClass())
		{
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
			UInventorySubsystem* InventorySubsystem = GameInstance->GetSubsystem<UInventorySubsystem>();

			// Load the item from the inventory subsystem
			InventorySlot.ItemObject = InventorySubsystem->GetItemById(InventorySlot.ItemId);
		}
	}
}




// Helper functions

int32 UInventoryComponent::AddUniqueItem(TScriptInterface<IInventoryItem> ItemObject)
{
	FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemObject);

	if (ExistingSlot)
	{
		// We can't add the item if it's already in the inventory
		return 0;
	}

	// We can drop into duplicate item behavior if there is no existing item
	return AddNewItem(ItemObject);
}

int32 UInventoryComponent::AddNewItem(TScriptInterface<IInventoryItem> ItemObject, int32 Count, int32 MaxStackSize)
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
	Inventory.Emplace(ItemObject, CountAdded);
	return CountAdded;
}

int32 UInventoryComponent::AddStackItem(TScriptInterface<IInventoryItem> ItemObject, int32 Count, int32 MaxStackSize)
{
	TArray<FInventorySlot*> ExistingSlots = FindAllItemSlots(ItemObject);
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
		const int32 NewStackCount = AddNewItem(ItemObject, Count - TotalCountAdded, MaxStackSize);
		
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

int32 UInventoryComponent::AddUniqueStackItem(TScriptInterface<IInventoryItem> ItemObject, int32 Count, int32 MaxStackSize)
{
	FInventorySlot* ExistingSlot = FindFirstItemSlot(ItemObject);

	if (ExistingSlot)
	{
		// We can't create another stack, so we can only add as much as the existing stack can hold
		return ExistingSlot->AddToStack(Count, MaxStackSize);
	}

	// Add a new stack slot and return it's size
	const int32 NewStackSize = FMath::Min(Count, MaxStackSize);
	Inventory.Emplace(ItemObject, NewStackSize);
	return NewStackSize;
}

