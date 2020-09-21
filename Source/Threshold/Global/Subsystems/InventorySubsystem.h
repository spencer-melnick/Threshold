// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventorySubsystem.generated.h"


// Forward declarations

class UInventoryTableItem;


/**
 * Subsystem that loads inventory objects created via other assets, such as from a data table. Scans the paths
 * on game start and produces stably named objects that support network replication.
 */
UCLASS(Config=Game)
class UInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


	// Accessors

	UInventoryTableItem* GetItemByName(FName ItemName)
	{
		UInventoryTableItem** FindResult = InventoryTableItems.Find(ItemName);
		if (!FindResult)
		{
			return nullptr;
		}

		return *FindResult;
	}


protected:
	// Helper functions

	void LoadTableItems();
	

private:
	UPROPERTY(Config)
	TArray<FString> InventoryTablePaths;

	UPROPERTY()
	TMap<FName, UInventoryTableItem*> InventoryTableItems;
};
