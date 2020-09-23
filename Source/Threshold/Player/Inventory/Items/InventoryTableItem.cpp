// Copyright (c) 2020 Spencer Melnick

#include "InventoryTableItem.h"

#include "Threshold/Threshold.h"


// FInventoryTableItem

// Inventory item overrides

bool FInventoryTableItem::CanHaveDuplicates() const
{
	const FInventoryTableRow* Row = GetRow();
	if (!Row)
	{
		return Super::CanHaveDuplicates();
	}

	return Row->bCanHaveDuplicates;
}


bool FInventoryTableItem::CanStack() const
{
	const FInventoryTableRow* Row = GetRow();
	if (!Row)
	{
		return Super::CanStack();
	}

	return Row->bCanStack;
}

FGameplayTagContainer FInventoryTableItem::GetGameplayTags()
{
	const FInventoryTableRow* Row = GetRow();
	if (!Row)
	{
		// Fall back to default behavior
		return Super::GetGameplayTags();
	}

	return Row->GameplayTags;
}

TSoftClassPtr<AActor> FInventoryTableItem::GetPreviewActorClass()
{
	const FInventoryTableRow* Row = GetRow();
	if (!Row)
	{
		return Super::GetPreviewActorClass();
	}

	return Row->PreviewActorClass;
}



// Network overrides

bool FInventoryTableItem::operator==(const FInventoryItem& Other) const
{
	if (!Super::operator==(Other))
	{
		return false;
	}

	const FInventoryTableItem& OtherTableItem = static_cast<const FInventoryTableItem&>(Other);
	return TableRowHandle == OtherTableItem.TableRowHandle;
}

bool FInventoryTableItem::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	TSoftObjectPtr<UDataTable> DataTablePointer;
	uint32 RowNameHash;

	if (Ar.IsSaving())
	{
		// Store a soft pointer reference to our data table
		DataTablePointer = TableRowHandle.DataTable;
		RowNameHash = HashInventoryRowName(TableRowHandle.RowName);
	}

	Ar << DataTablePointer;
	Ar << RowNameHash;

	if (Ar.IsLoading())
	{
		if (!DataTablePointer.IsValid())
		{
			// Check for a valid data table pointer
			UE_LOG(LogThresholdGeneral, Warning, TEXT("Inventory table item received empty table on net deserialize"))

			// Clear row handle
			TableRowHandle.DataTable = nullptr;
			TableRowHandle.RowName = NAME_None;
			
			bOutSuccess = true;
			return true;
		}
		
		UDataTable* NewDataTable = DataTablePointer.LoadSynchronous();
		bool bChangedDataTable = false;

		if (TableRowHandle.DataTable != NewDataTable)
		{
			// Validate our new data table if we have one
			if (NewDataTable->RowStruct != FInventoryTableRow::StaticStruct())
			{
				UE_LOG(LogThresholdGeneral, Error, TEXT("Inventory table item received table %s with invalid struct type %s on net deserialize"),
                    *NewDataTable->GetPathName(), *GetNameSafe(NewDataTable->RowStruct))
				bOutSuccess = false;
				return false;
			}

			bChangedDataTable = true;
			TableRowHandle.DataTable = NewDataTable;
		}

		// If we changed the data table, or the received row name hash doesn't match our row name hash, we need to find a new item
		const bool bChangedRowName = bChangedDataTable || (RowNameHash != HashInventoryRowName(TableRowHandle.RowName));

		if (bChangedRowName)
		{
			FName FoundRowName = NAME_None;

			for (auto& Entry : TableRowHandle.DataTable->GetRowMap())
			{
				// Try to find our row name by hash
				if (HashInventoryRowName(Entry.Key) == RowNameHash)
				{
					FoundRowName = Entry.Key;
					break;
				}
			}

			if (FoundRowName == NAME_None)
			{
				UE_LOG(LogThresholdGeneral, Error, TEXT("Inventory table item received table %s but could not find matching row name on net deserialize"),
                    *TableRowHandle.DataTable->GetPathName())
				bOutSuccess = false;
				return false;
			}

			TableRowHandle.RowName = FoundRowName;
		}
	}

	bOutSuccess = true;
	return true;
}

