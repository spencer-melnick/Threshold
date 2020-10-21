// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "ItemType.generated.h"



/**
 * Basic inventory item type, with properties accessible in the editor
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryItemType : public UInventoryItemTypeBase
{
	GENERATED_BODY()


public:

	// Item type base overrides
	
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
	virtual FText GetItemName(TWeakPtr<FInventoryItemDataBase> ItemData) const override { return ItemName; }
	virtual FText GetItemDescription(TWeakPtr<FInventoryItemDataBase> ItemData) const override { return ItemDescription; }
	virtual TSoftClassPtr<APreviewActor> GetPreviewActorClass(TWeakPtr<FInventoryItemDataBase> ItemData) const override { return PreviewActorClass; }
	virtual TSoftObjectPtr<UTexture2D> GetThumbnailImage(TWeakPtr<FInventoryItemDataBase> ItemData) const override { return ThumbnailImage; }
	virtual FGameplayTagContainer GetGameplayTags(TWeakPtr<FInventoryItemDataBase> ItemData) const override { return GameplayTags; }
	virtual bool AllowsDuplicates() const override { return bAllowsDuplicates; }


	// Operator overloads

	virtual bool operator==(const UInventoryItemTypeBase& OtherType) const override;
	bool operator==(const UInventoryItemType& OtherType) const;


	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	TSoftClassPtr<APreviewActor> PreviewActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	TSoftObjectPtr<UTexture2D> ThumbnailImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	bool bAllowsDuplicates = true;
};
