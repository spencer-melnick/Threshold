// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Threshold/Player/Inventory/InventoryItem.h"
#include "InventoryTableItem.generated.h"



/**
 * Data table row for simple inventory items
 */
USTRUCT()
struct FInventoryTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	bool bCanHaveDuplicates = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	bool bCanStack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	TSoftClassPtr<AActor> PreviewActorClass;
};


/**
 * Simple inventory item that reads its properties from a data table
 */
USTRUCT(BlueprintType)
struct FInventoryTableItem : public FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle TableRowHandle;


	// Inventory item overrides

	FInventoryTableRow* GetRow() const { return TableRowHandle.GetRow<FInventoryTableRow>(TEXT("InventoryTableItem")); }
	virtual bool CanHaveDuplicates() const override;
	virtual bool CanStack() const override;
	virtual FGameplayTagContainer GetGameplayTags() override;
	virtual TSoftClassPtr<AActor> GetPreviewActorClass() override;
	virtual bool IsValid() const override { return GetRow() != nullptr; }


	// Network overrides
	
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	virtual bool operator==(const FInventoryItem& Other) const override;
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};



/**
 * Helper for inventory table item
 */
UCLASS()
class UInventoryTableItemHelper : public UInventoryItemHelper
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInventoryTableItem Item;

	virtual FInventoryItem* GetItem() override { return &Item; }
};


// Simple helper function to do (hopefully) platform independent hashes of FNames by their string values
inline uint32 HashInventoryRowName(FName Name)
{
	const FTCHARToUTF8 StringUtf8(*Name.ToString());
	return CityHash32(StringUtf8.Get(), StringUtf8.Length());
}



// Enable net serialization

template<>
struct TStructOpsTypeTraits< FInventoryTableItem > : public TStructOpsTypeTraitsBase2< FInventoryTableItem >
{
	enum 
	{
		WithNetSerializer = true,
    };
};
