// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
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
};



/**
 * Simple item type that references a data table row
 */
UCLASS(EditInlineNew)
class INVENTORYSYSTEM_API UTableItem : public UItemTypeBase
{
	GENERATED_BODY()

public:

	// Item type overrides
	
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
	virtual FText GetItemName(TWeakPtr<FItemDataBase, ESPMode::Fast> ItemData) const override;
	virtual FText GetItemDescription(TWeakPtr<FItemDataBase, ESPMode::Fast> ItemData) const override;
	virtual TSoftClassPtr<AActor> GetPreviewActorClass(TWeakPtr<FItemDataBase, ESPMode::Fast> ItemData) const override;


	// Editor properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle RowHandle;

protected:
	FItemRow* GetRow() const;
	static uint32 HashName(FName Name)
	{
		const FTCHARToUTF8 StringUtf8(*Name.ToString());
		return CityHash32(StringUtf8.Get(), StringUtf8.Length());
	}
};
