// Copyright (c) 2020 Spencer Melnick

#include "Inventory/ItemTypes/TableItem.h"
#include "InventorySystem.h"


// UTableItem

// Item type overrides

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

		if (!RowHandle.DataTable || RowHandle.DataTable->RowStruct != FItemRow::StaticStruct())
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

	if (!ItemRow)
	{
		return FText();
	}

	return ItemRow->ItemName;
}

FText UTableInventoryItem::GetItemDescription(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const
{
	const FItemRow* ItemRow = GetRow();

	if (!ItemRow)
	{
		return FText();
	}

	return ItemRow->ItemDescription;
}

TSoftClassPtr<AActor> UTableInventoryItem::GetPreviewActorClass(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const
{
	const FItemRow* ItemRow = GetRow();

	if (!ItemRow)
	{
		return nullptr;
	}

	return ItemRow->PreviewActorClass;
}


// Protected helpers

FItemRow* UTableInventoryItem::GetRow() const
{
	return RowHandle.GetRow<FItemRow>(TEXT("UTableItem"));
}
