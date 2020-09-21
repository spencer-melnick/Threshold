// Copyright (c) 2020 Spencer Melnick

#include "InventorySubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/ObjectLibrary.h"
#include "Threshold/Threshold.h"
#include "Threshold/Player/Inventory/Items/InventoryTableItem.h"


// UInventorySubsystem

void UInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	LoadTableItems();
}



// Helper functions

void UInventorySubsystem::LoadTableItems()
{
	// Needs some path by default
	if (InventoryTablePaths.Num() == 0)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("InventorySubsystem has no specified paths, scanning entire directory"))
		InventoryTablePaths.Add(TEXT("/Game"));
	}
	
	// Load objects from paths
	UObjectLibrary* TableObjectLibrary = UObjectLibrary::CreateLibrary(UDataTable::StaticClass(), false, true);
	TableObjectLibrary->bRecursivePaths = true;
	const int32 TableCount = TableObjectLibrary->LoadAssetsFromPaths(InventoryTablePaths);

	// Store the table objects in our array
	TArray<UDataTable*> DataTables;
	DataTables.Reserve(TableCount);
	TableObjectLibrary->GetObjects(DataTables);

	InventoryTableItems.Reserve(TableCount);

	for (const UDataTable* DataTable : DataTables)
	{
		// Skip data tables with the wrong type
		if (DataTable->GetRowStruct() != FInventoryItemRow::StaticStruct())
		{
			continue;
		}

		for (const TPair<FName, uint8*> Row : DataTable->GetRowMap())
		{
			const FInventoryItemRow* RowData = reinterpret_cast<FInventoryItemRow*>(Row.Value);
			const FName RowName = Row.Key;

			if (InventoryTableItems.Contains(RowName))
			{
				// Skip objects with the same name
				UE_LOG(LogThresholdGeneral, Warning, TEXT("Duplicate inventory item %s found in table %s - skipping"),
					*RowName.ToString(), *DataTable->GetName())
				break;
			}

			// Load the item properties from the row struct
			UInventoryTableItem* NewItem = NewObject<UInventoryTableItem>(this, RowName);
			NewItem->StorageBehavior = RowData->StorageBehavior;
			NewItem->MaxStackSize = RowData->MaxStackSize;
			NewItem->GameplayTags = RowData->GameplayTags;
			NewItem->PreviewActorClass = RowData->PreviewActorClass;

			// Add the item to the list
			InventoryTableItems.Add(RowName, NewItem);
		}
	}

	UE_LOG(LogThresholdGeneral, Display, TEXT("Loaded %d inventory items from tables"), InventoryTableItems.Num())
}

