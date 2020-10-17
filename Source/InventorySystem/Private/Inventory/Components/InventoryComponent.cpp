// Copyright (c) 2020 Spencer Melnick

#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/ItemTypes/ItemTypeBase.h"
#include "Inventory/InventoryItem.h"
#include "Net/UnrealNetwork.h"

// UInventoryComponent

UInventoryComponent::UInventoryComponent()
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

int32 UInventoryComponent::AddItem(const FInventoryItem& NewItem)
{
	if (!NewItem.IsValid())
	{
		return 0;
	}

	UInventoryItemTypeBase* NewItemType = NewItem.GetType();
	TArray<FInventoryItem*> CurrentItems = GetAllItemsByType(NewItemType);

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
			
			for (FInventoryItem* ExistingStack : CurrentItems)
			{
				// Try to add to the existing stacks until there is nothing left to add
				CountLeftToAdd -= ExistingStack->AddToStack(CountLeftToAdd);

				if (CountLeftToAdd <= 0)
				{
					// Early exit if there is nothing left to add
					return InitialCount;
				}
            }
		}
	}

	if (CurrentItems.Num() > 0)
	{
		if (!NewItemType->AllowsDuplicates())
		{
			// If the item doesn't allow duplicates, return however many are left to add
			return InitialCount - CountLeftToAdd;
		}
	}

	// Add a new item by copy
	FInventoryItem& ItemCopy = InventoryArray.Items.Emplace_GetRef(NewItem);
	InventoryArray.MarkItemDirty(ItemCopy);

	if (NewItemType->AllowsStacking())
	{
		// If the item can be stacked, set the stack size to the remaining count
		ItemCopy.SetStackCount(CountLeftToAdd);
		return InitialCount;
	}

	// If it wasn't a stack item, we only added one
	return 1;
}

int32 UInventoryComponent::RemoveItem(UInventoryItemTypeBase* ItemType, int32 Count)
{
	if (!ItemType)
	{
		return 0;
	}

	int32 CountLeftToRemove = Count;

	if (ItemType->AllowsStacking())
	{
		// If the item is stackable, try and remove it from the stacks
		TArray<FInventoryItem*> ExistingStacks = GetAllItemsByType(ItemType);

		for (FInventoryItem* ExistingStack : ExistingStacks)
		{
			// Remove until there are no more to remove
			CountLeftToRemove -= ExistingStack->RemoveFromStack(CountLeftToRemove);

			if (CountLeftToRemove <= 0)
			{
				// Early exit if we've already removed everything
				break;
			}
		}

		InventoryArray.Items.RemoveAll([](const FInventoryItem& Item) -> bool
		{
			// Remove all empty stacks
			return Item.GetStackCount() == 0;
		});
	}
	else
	{
		InventoryArray.Items.RemoveAll([&CountLeftToRemove, ItemType](const FInventoryItem& Item) -> bool
		{
			if (CountLeftToRemove <= 0 || !Item.IsValid() || *ItemType != *Item.GetType())
			{
				// If we already removed everything we need, if the item is invalid or the type doesn't match, skip removal
				return false;
			}

			// Decrement the counter
			CountLeftToRemove--;
			return true;
		});
	}

	return CountLeftToRemove;
}

TArray<FInventoryItem*> UInventoryComponent::GetAllItemsByType(UInventoryItemTypeBase* ItemType)
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
		FInventoryItem* SingleResult = GetFirstItemByType(ItemType);

		if (SingleResult)
		{
			Result.Add(SingleResult);
		}
	}
	else
	{
		for (FInventoryItem& InventoryItem : InventoryArray.Items)
		{
			// Add each item pointer if its type matches
			if (*InventoryItem.GetType() == *ItemType)
			{
				Result.Add(&InventoryItem);
			}
		}
	}

	return Result;
}

FInventoryItem* UInventoryComponent::GetFirstItemByType(UInventoryItemTypeBase* ItemType)
{
	if (!ItemType)
	{
		return nullptr;
	}
	
	return InventoryArray.Items.FindByPredicate([ItemType](const FInventoryItem& Item)
	{
		return *Item.GetType() == *ItemType; 
	});
}




// Network replication

void UInventoryComponent::OnRep_InventoryArray()
{
	
}

