// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Threshold/Player/Inventory/InventoryItem.h"
#include "UObject/StrongObjectPtr.h"

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

	TScriptInterface<IInventoryItem> GetItemByName(FName Name);
	TScriptInterface<IInventoryItem> GetItemById(uint32 Id);
	static uint32 HashName(FName Name);


protected:
	// Helper functions

	void LoadTableItems();
	

private:
	UPROPERTY(Config)
	TArray<FString> InventoryTablePaths;

	UPROPERTY()
	TMap<uint32, TScriptInterface<IInventoryItem>> InventoryTableItems;
};
