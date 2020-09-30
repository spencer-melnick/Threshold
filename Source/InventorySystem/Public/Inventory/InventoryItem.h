// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.generated.h"


// Forward declarations

class UInventoryItemTypeBase;
struct FInventoryItemDataBase;


/**
 * Class used for storing actual inventory items as they appear in the inventory. Specifically designed to support
 * replication via an inventory system component. As UObjects cannot be replicated directly (only references to package
 * loaded objects), this struct handles replicating the mutable properties of the UObjects.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend class FInventoryItemDetails;

public:

	// Property accessors

	/**
	 * Used to get the name of this item as it should appear in the inventory UI
	 */
	FText GetName() const;

	/**
	 * Used to get the description of this item as it should appear in the inventory UI
	*/
	FText GetDescription() const;

	/**
	 * Used to get an actor that can be rendered as a 3D display for this inventory item
	 */
	TSoftClassPtr<AActor> GetPreviewActorClass() const;


	// Data accessors

	UInventoryItemTypeBase* GetType() const { return Type; }
	TWeakPtr<FInventoryItemDataBase> GetData() const { return Data; }
	void SetType(UInventoryItemTypeBase* NewType);


	// Serialization
	
	bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess);
	bool Serialize(FArchive& Ar);


private:
	// Storage
	
	UPROPERTY(EditAnywhere, Instanced, meta=(AllowPrivateAccess="true"))
	UInventoryItemTypeBase* Type = nullptr;

	TSharedPtr<FInventoryItemDataBase> Data;
};



/**
 * Type traits for the struct to tell the engine to use custom serialization functions
 */
template <>
struct TStructOpsTypeTraits<FInventoryItem> : TStructOpsTypeTraitsBase2<FInventoryItem>
{
	enum
	{
		WithSerializer = true,
		WithNetSerializer = true
	};
};
