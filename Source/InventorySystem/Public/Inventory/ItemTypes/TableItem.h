// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
#include "Inventory/DataTypes/StackData.h"
#include "Engine/DataTable.h"
#include "TableItem.generated.h"


USTRUCT()
struct FItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText ItemName;

	UPROPERTY(EditAnywhere)
	FText ItemDescription;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> PreviewActorClass;

	UPROPERTY(EditAnywhere)
	bool bAllowsDuplicates = false;
};



/**
 * Simple item type that references a data table row
 */
UCLASS(EditInlineNew)
class INVENTORYSYSTEM_API UTableInventoryItem : public UInventoryItemTypeBase
{
	GENERATED_BODY()

public:

	// Item type overrides
	
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
	virtual FText GetItemName(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const override;
	virtual FText GetItemDescription(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const override;
	virtual TSoftClassPtr<AActor> GetPreviewActorClass(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData) const override;
	virtual bool AllowsDuplicates() const override;


	virtual UScriptStruct* GetRowStruct() const { return FItemRow::StaticStruct(); }


	// Editor properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle RowHandle;

protected:
	template <typename RowType = FItemRow>
	RowType* GetRow() const
	{
		static_assert(TIsDerivedFrom<RowType, FItemRow>::IsDerived, "Table item row type should be derived from FItemRow");
		return RowHandle.GetRow<RowType>(TEXT("UTableInventoryItem"));
	}
	
	static uint32 HashName(FName Name)
	{
		const FTCHARToUTF8 StringUtf8(*Name.ToString());
		return CityHash32(StringUtf8.Get(), StringUtf8.Length());
	}
};


USTRUCT()
struct FStackItemRow : public FItemRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize = 99;
};

UCLASS()
class INVENTORYSYSTEM_API UTableStackItem : public UTableInventoryItem
{
	GENERATED_BODY()
public:

	virtual UScriptStruct* GetItemDataType() const override { return FInventoryStackData::StaticStruct(); }
	virtual TSharedPtr<FInventoryItemDataBase> CreateItemData() const override { return MakeShareable(new FInventoryStackData()); }
	virtual bool AllowsStacking() const override { return true; }
	virtual int32 AddToStack(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const override;
	virtual int32 RemoveFromStack(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const override;
	virtual void SetStackCount(TWeakPtr<FInventoryItemDataBase, ESPMode::Fast> ItemData, const int32 Count) const override;
	virtual int32 GetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData) const override;
	int32 GetMaxStackSize() const;
};
