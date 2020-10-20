﻿// Copyright (c) 2020 Spencer Melnick

#include "Inventory/InventoryArray.h"



// FInventoryArrayHandle

// Accessors

int32 FInventoryArrayHandle::GetIndex() const
{
	if (IsNull())
	{
		return INDEX_NONE;
	}

	return Array->LookupIndex(ItemID);
}

FInventoryItem* FInventoryArrayHandle::Get() const
{
	const int32 Index = GetIndex();

	if (Index == INDEX_NONE)
	{
		return nullptr;
	}

	check(Index < Array->Items.Num());
	return &Array->Items[Index];
}

void FInventoryArrayHandle::MarkDirty() const
{
	FInventoryItem* Item = Get();

	if (!Item)
	{
		return;
	}

	Array->MarkItemDirty(*Item);
}

void FInventoryArrayHandle::Remove()
{
	if (IsNull())
	{
		return;
	}

	Array->Remove(*this);
}



// FInventoryArray

void FInventoryArray::PostSerialize(FArchive& Ar)
{
	if (Ar.IsLoading())
	{
		// If we loaded from something, rebuild the ID map!
		RebuildIDMap();
		NotifyArrayChanged();
	}
}


void FInventoryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (AddedIndices.Num() > 0)
	{
		// If we received any additions, we need to rebuild the local ID map
		bNeedsIDRebuild = true;
	}
}


void FInventoryArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	if (RemovedIndices.Num() > 0)
	{
		bNeedsIDRebuild = true;
	}
}



bool FInventoryArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	const bool Result = FastArrayDeltaSerialize(Items, DeltaParams, *this);

	if (bNeedsIDRebuild)
	{
		// If we added or deleted any elements, rebuild the ID map
		RebuildIDMap();
		NotifyArrayChanged();
		bNeedsIDRebuild = false;
	}

	return Result;
}



// Array operations

void FInventoryArray::Remove(FInventoryArrayHandle& ItemHandle)
{
	if (ItemHandle.IsNull() || ItemHandle.Array != this)
	{
		return;
	}

	const int32 Index = LookupIndex(ItemHandle.ItemID);
	
	if (Index == INDEX_NONE)
	{
		return;
	}

	// Delete the item
	check(Index < Items.Num());
	Items.RemoveAt(Index);
	NotifyItemsDeleted();
}

void FInventoryArray::Empty(int32 Slack)
{
	Items.Empty(Slack);
	IDtoIndex.Empty();
	MarkArrayDirty();
	InventoryArrayChangedDelegate.Broadcast();
}



// State changes

void FInventoryArray::NotifyArrayChanged()
{
	InventoryArrayChangedDelegate.Broadcast();
}


void FInventoryArray::NotifyItemsDeleted()
{
	// Update the array state and notify any listeners
	MarkArrayDirty();
	bNeedsIDRebuild = true;
	NotifyArrayChanged();
}



// Helper functions

void FInventoryArray::RebuildIDMap()
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].UniqueID == INDEX_NONE)
		{
			// If an item doesn't have an ID yet, assign it one
			Items[i].UniqueID = GetNextID();
		}
		
		IDtoIndex.Emplace(Items[i].UniqueID, i);
	}
}

int32 FInventoryArray::LookupIndex(const int32 UniqueID)
{
	if (bNeedsIDRebuild)
	{
		// Rebuild the ID map if any operation marked it dirty
		RebuildIDMap();
		bNeedsIDRebuild = false;
	}

	int32* FoundIndex = IDtoIndex.Find(UniqueID);

	if (!FoundIndex)
	{
		return INDEX_NONE;
	}

	return *FoundIndex;
}
