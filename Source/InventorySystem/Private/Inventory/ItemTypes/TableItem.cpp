// Copyright (c) 2020 Spencer Melnick

#include "Inventory/ItemTypes/TableItem.h"
/*#include "InventorySystem.h"
#include "Inventory/DataTypes/ItemData.h"
#include "Inventory/DataTypes/StackData.h"


// UTableItem

// Item type overrides

/*

bool UTableInventoryItem::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	TSoftObjectPtr<UDataTable> TablePointer;
	uint32 RowNameHash;
	
	if (Ar.IsSaving())
	{
		const FItemRow* ItemRow = GetRow();
		if (!ItemRow)
		{
			UE_LOG(LogInventorySystem, Error, TEXT("UTableItem::NetSerialize failed on %s - invalid row handle"),
				*GetNameSafe(this))
			bOutSuccess = false;
			return true;
		}
		
		// Save a soft pointer to the data table and the row name hash
		TablePointer = RowHandle.DataTable;
		RowNameHash = HashName(RowHandle.RowName);
	}

	Ar << TablePointer;
	Ar << RowNameHash;

	if (Ar.IsLoading())
	{
		// Load synchronously since we will need the data table immediately
		// TODO: add method for loading synchronously without flushing all pending asynchronous loads
		RowHandle.DataTable = TablePointer.LoadSynchronous();

		if (!RowHandle.DataTable || RowHandle.DataTable->RowStruct != GetRowStruct())
		{
			UE_LOG(LogInventorySystem, Error, TEXT("UTableItem::NetSerialize failed on %s - received invalid data table reference %s"),
				*GetNameSafe(this), *TablePointer->GetPathName())
			bOutSuccess = false;
			return true;
		}

		for (auto& RowEntry : RowHandle.DataTable->GetRowMap())
		{
			if (HashName(RowEntry.Key) == RowNameHash)
			{
				// Find our row name by its hash
				RowHandle.RowName = RowEntry.Key;
				bOutSuccess = true;
				return true;
			}
		}

		UE_LOG(LogInventorySystem, Error, TEXT("UTableItem::NetSerialize failed on %s - received invalid row name hash"),
            *GetNameSafe(this))
		bOutSuccess = false;
		return true;
	}

	bOutSuccess = true;
	return true;
}

FText UTableInventoryItem::GetItemName(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const
{
	const FItemRow* ItemRow = GetRow();
	return ItemRow ? ItemRow->ItemName : FText();
}

FText UTableInventoryItem::GetItemDescription(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const
{
	const FItemRow* ItemRow = GetRow();
	return ItemRow ? ItemRow->ItemDescription : FText();
}

TSoftClassPtr<APreviewActor> UTableInventoryItem::GetPreviewActorClass(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const
{
	const FItemRow* ItemRow = GetRow();
	return ItemRow ? ItemRow->PreviewActorClass : nullptr;
}

bool UTableInventoryItem::AllowsDuplicates() const
{
	const FItemRow* ItemRow = GetRow();
	return ItemRow ? ItemRow->bAllowsDuplicates : false;
}

bool UTableInventoryItem::operator==(const UInventoryItemTypeBase& OtherType)
{
	if (!Super::operator==(OtherType))
	{
		// Super operator == should check class types for equality already
		return false;
	}

	// Compare table items by row handle
	const UTableInventoryItem* OtherTableType = Cast<UTableInventoryItem>(&OtherType);
	check(OtherTableType);
	return OtherTableType->RowHandle == RowHandle;
}




// UTableStackItem

int32 UTableStackItem::AddToStack(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const
{
	const int32 PreviousStackCount = GetStackCount(ItemData);
	const int32 NewStackCount = FMath::Clamp(PreviousStackCount + Count, 0, GetMaxStackSize());
	SetStackCount(ItemData, NewStackCount);
	return NewStackCount - PreviousStackCount;
}

int32 UTableStackItem::RemoveFromStack(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const
{
	return -AddToStack(ItemData, -Count);
}

void UTableStackItem::SetStackCount(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);

	if (!StackData.IsValid())
	{
		return;
	}

	StackData->StackCount = Count;
}


int32 UTableStackItem::GetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);
	return StackData.IsValid() ? StackData->StackCount : 0;
}

int32 UTableStackItem::GetMaxStackSize() const
{
	const FStackItemRow* ItemRow = GetRow<FStackItemRow>();
	return ItemRow ? ItemRow->MaxStackSize : 1;
} */
