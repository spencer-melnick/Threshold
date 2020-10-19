// Copyright (c) 2020 Spencer Melnick

#include "Inventory/Components/InventoryComponent.h"

#include "InventorySystem.h"
#include "Inventory/ItemTypes/ItemTypeBase.h"
#include "Inventory/InventoryItem.h"
#include "Net/UnrealNetwork.h"



// UInventoryComponent

UInventoryComponent::UInventoryComponent()
	: InventoryArray(this)
{
	// Should be replicated by default
	SetIsReplicatedByDefault(true);
}



// Engine overrides

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryArray);
}



// Inventory access

UInventoryComponent::FAdditionResult UInventoryComponent::AddItem(const FInventoryItem& NewItem)
{
	if (!NewItem.IsValid())
	{
		return FAdditionResult(0, FInventoryArrayHandle());
	}

	UInventoryItemTypeBase* NewItemType = NewItem.GetType();
	TArray<FInventoryArrayHandle> CurrentItems = GetAllItemsByType(NewItemType);

	int32 InitialCount = 1;
	int32 CountLeftToAdd = InitialCount;

	if (NewItem.AllowsStacking())
	{
		// If the item allows stacking, figure out how many we're trying to store
		InitialCount = NewItem.GetStackCount();
		CountLeftToAdd = InitialCount;

		if (CurrentItems.Num() > 0)
		{
			// If there are any existing stacks
			for (FInventoryArrayHandle ExistingStack : CurrentItems)
			{
				// Try to add to the existing stacks until there is nothing left to add
				const int32 CountAdded = ExistingStack->AddToStack(CountLeftToAdd);

				if (CountAdded > 0)
				{
					// Mark the item dirty on any change
					CountLeftToAdd -= CountAdded;
					ExistingStack.MarkDirty();
				}

				if (CountLeftToAdd <= 0)
				{
					// Early exit if there is nothing left to add
					return FAdditionResult(InitialCount, FInventoryArrayHandle());
				}
            }
		}
	}

	if (CurrentItems.Num() > 0)
	{
		if (!NewItemType->AllowsDuplicates())
		{
			// If the item doesn't allow duplicates, return however many are left to add
			return FAdditionResult(InitialCount - CountLeftToAdd, FInventoryArrayHandle());
		}
	}

	// Add a new item by copy
	FInventoryArrayHandle ItemCopy = InventoryArray.Emplace(NewItem);

	if (NewItemType->AllowsStacking() && ItemCopy->IsValid())
	{
		// If the item can be stacked, set the stack size to the remaining count
		ItemCopy->SetStackCount(CountLeftToAdd);
		ItemCopy.MarkDirty();
	}
	
	return TPair<int32, FInventoryArrayHandle>(InitialCount, ItemCopy);
}

int32 UInventoryComponent::RemoveItem(UInventoryItemTypeBase* ItemType, int32 Count)
{
	if (!ItemType || Count <= 0)
	{
		return 0;
	}

	int32 CountLeftToRemove = Count;
	
	if (ItemType->AllowsStacking())
	{
		// If the item is stackable, try and remove it from the stacks
		TArray<FInventoryArrayHandle> ExistingStacks = GetAllItemsByType(ItemType);

		for (FInventoryArrayHandle ExistingStack : ExistingStacks)
		{
			// Remove until there are no more to remove
			const int32 CountRemoved = ExistingStack->RemoveFromStack(CountLeftToRemove);

			if (CountRemoved > 0)
			{
				// If anything changed mark the existing stack as dirty
				CountLeftToRemove -= ExistingStack->RemoveFromStack(CountLeftToRemove);
				ExistingStack.MarkDirty();
			}

			if (CountLeftToRemove <= 0)
			{
				// Early exit if we've already removed everything
				break;
			}
		}

		InventoryArray.RemoveAll([](const FInventoryItem& Item) -> bool
		{
			// Remove all empty stacks
			return Item.GetStackCount() == 0;
		});
	}
	else
	{
		// Get all items by handle so we can delete them while iterating
		// (a bit slower, but cleaner)
		TArray<FInventoryArrayHandle> ExistingItems = GetAllItemsByType(ItemType);

		for (FInventoryArrayHandle ItemHandle : ExistingItems)
		{
			if (*ItemHandle->GetType() != *ItemType)
			{
				continue;
			}
			
			ItemHandle.Remove();

			if (CountLeftToRemove-- <= 0)
			{
				// Decrement the count left to remove and exit if we've finished
				break;
			}
		}
	}

	return CountLeftToRemove;
}

TArray<FInventoryItem*> UInventoryComponent::GetAllItemsByTypeTemporary(UInventoryItemTypeBase* ItemType)
{
	TArray<FInventoryItem*> Result;

	if (!ItemType)
	{
		// Return an empty result if the type is invalid
		return Result;
	}

	if (!ItemType->AllowsDuplicates())
	{
		// Find only the first result if the item type doesn't allow duplicates
		FInventoryItem* SingleResult = GetFirstItemByTypeTemporary(ItemType);

		if (SingleResult)
		{
			Result.Add(SingleResult);
		}

		return Result;
	}
	else
	{
		return InventoryArray.FindAllTemporary([ItemType](const FInventoryItem& Item) -> bool
        {
            // Add each item pointer if its type matches
            return *Item.GetType() == *ItemType;
        });
	}
}

TArray<FInventoryArrayHandle> UInventoryComponent::GetAllItemsByType(UInventoryItemTypeBase* ItemType)
{
	TArray<FInventoryArrayHandle> Result;

	if (!ItemType)
	{
		// Return an empty result if the type is invalid
		return Result;
	}

	if (!ItemType->AllowsDuplicates())
	{
		// Find only the first result if the item type doesn't allow duplicates
		FInventoryArrayHandle SingleResult = GetFirstItemByType(ItemType);

		if (!SingleResult.IsNull())
		{
			Result.Add(SingleResult);
		}

		return Result;
	}
	else
	{
		return InventoryArray.FindAll([ItemType](const FInventoryItem& Item) -> bool
		{
			// Add each item pointer if its type matches
			return *Item.GetType() == *ItemType;
		});
	}
}

FInventoryItem* UInventoryComponent::GetFirstItemByTypeTemporary(UInventoryItemTypeBase* ItemType)
{
	if (!ItemType)
	{
		return nullptr;
	}
	
	return InventoryArray.FindTemporary([ItemType](const FInventoryItem& Item)
    {
        return *Item.GetType() == *ItemType; 
    });
}

FInventoryArrayHandle UInventoryComponent::GetFirstItemByType(UInventoryItemTypeBase* ItemType)
{
	if (!ItemType)
	{
		return FInventoryArrayHandle();
	}
	
	return InventoryArray.Find([ItemType](const FInventoryItem& Item)
	{
		return *Item.GetType() == *ItemType; 
	});
}




// Network replication

void UInventoryComponent::OnRep_InventoryArray()
{
	
}
