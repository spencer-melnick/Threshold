// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Threshold/Player/Inventory/InventoryItem.h"
#include "InventoryTableItem.generated.h"


/**
 * Holds an inventory object created via a data table. Only intended for usage by the inventory subsystem
 */
UCLASS()
class UInventoryTableItem : public UObject, public IInventoryItem
{
	GENERATED_BODY()

	friend class UInventorySubsystem;

public:
	// Inventory item overrides

	virtual EInventoryStorageBehavior GetStorageBehavior() const override { return StorageBehavior; }
	virtual int32 GetMaxStackSize() const override { return MaxStackSize; }
	virtual FGameplayTagContainer GetGameplayTags() const override { return GameplayTags; }
	virtual TSoftClassPtr<AActor> GetPreviewActorClass() const override { return PreviewActorClass; };


	// Engine overrides

	virtual bool IsNameStableForNetworking() const override { return true; }

private:
	EInventoryStorageBehavior StorageBehavior;
	int32 MaxStackSize;
	FGameplayTagContainer GameplayTags;
	TSoftClassPtr<AActor> PreviewActorClass;
};


/**
 * Struct used for storing item data in a table
 */
USTRUCT(BlueprintType)
struct FInventoryItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemTableData)
	EInventoryStorageBehavior StorageBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemTableData)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemTableData)
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemTableData)
	TSoftClassPtr<AActor> PreviewActorClass;
};
