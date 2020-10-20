// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/DataTypes/ItemData.h"
#include "Inventory/ItemTypes/ItemTypeBase.h"
#include "InventoryItem.generated.h"


// Forward declarations

class UInventoryItemTypeBase;
class APreviewActor;



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
	friend class UInventoryComponent;
	friend struct FInventoryArray;

public:
	FInventoryItem() {}


	/**
	 * Type constructor
	 */
	FInventoryItem(UInventoryItemTypeBase* Type) :
		Type(Type)
	{
		if (Type)
		{
			Data = Type->CreateItemData();
		}
	}

	
	/**
	 * Copy constructor
	 */
	FInventoryItem(const FInventoryItem& OtherItem) :
		Type(OtherItem.Type), UniqueID(OtherItem.UniqueID)
	{
		if (OtherItem.Data.IsValid())
		{
			Data = TSharedPtr<FInventoryItemDataBase>(OtherItem.Data->Copy());
		}
	}


	/**
	 * Move constructor
	 */
	FInventoryItem(FInventoryItem&& OtherItem) noexcept :
		Type(OtherItem.Type), Data(MoveTemp(OtherItem.Data)), UniqueID(OtherItem.UniqueID) {}


	/**
	 * Copy assignment
	 */
	FInventoryItem& operator=(const FInventoryItem& OtherItem)
	{
		Type = OtherItem.Type;
		UniqueID = OtherItem.UniqueID;

		if (OtherItem.Data.IsValid())
		{
			Data = TSharedPtr<FInventoryItemDataBase>(OtherItem.Data->Copy());
		}

		return *this;
	}

	
	/**
	 * Move assignment
	 */
	FInventoryItem& operator=(FInventoryItem&& OtherItem) noexcept
	{
		Type = OtherItem.Type;
		Data = MoveTemp(OtherItem.Data);
		UniqueID = OtherItem.UniqueID;

		return *this;
	}
	

	
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
	TSoftClassPtr<APreviewActor> GetPreviewActorClass() const;

	/**
	 * Used to get an image for displaying a thumbnail of this inventory item
	 */
	TSoftObjectPtr<UTexture> GetThumbnailImage() const;

	/**
	 * Used to get gameplay tags describing this item
	*/
	virtual FGameplayTagContainer GetGameplayTags() const;

	/**
	* Used to determine inventory storage behavior
	* @return Whether or not an inventory component should store multiple items with this item type
	*/
	bool AllowsDuplicates() const;

	/**
	 * Used to determine inventory storage behavior
	 * @return Whether or not an inventory component should store multiple items with this item type
	 */
	bool AllowsStacking() const;

	/**
	 * Try to add items to a stack of this type
	 * @param Count - Number of items of this type to try to add to the stack
	 * @return Number of items added to the stack. 0 on failure
	 */
	int32 AddToStack(const int32 Count);

	/**
	 * Try to remove items from a stack of this type
	 * @param Count - Number of items of this type to try to remove from stack
	 * @return Number of items removed from the stack. 0 on failure
	 */
	int32 RemoveFromStack(const int32 Count);

	/**
	 * Try set the number of items in a stack of this type - does not need to check against max stack size
	 * @param Count - Number of items of this type try to set
	 */
	void SetStackCount(const int32 Count) const;

	/**
	 * Used to get the number of items in a stack of this type
	 * @return Number of items stored in the stack
	 */
	int32 GetStackCount() const;

	/**
	 * Check to see if the object is valid. The item is considered valid if the type is non-null and the item data
	 * type matches what is expected of the item type
	 * @return True if the item is valid, false otherwise
	 */
	bool IsValid() const;


	// Data accessors

	UInventoryItemTypeBase* GetType() const { return Type; }
	TWeakPtr<FInventoryItemDataBase> GetData() const { return Data; }
	void SetType(UInventoryItemTypeBase* NewType);


	// Serialization
	
	bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess);
	bool Serialize(FArchive& Ar);


private:
	// Storage
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	UInventoryItemTypeBase* Type = nullptr;

	TSharedPtr<FInventoryItemDataBase> Data = nullptr;


	// Unique ID for fast lookup in inventory component

	UPROPERTY(NotReplicated)
	int32 UniqueID = INDEX_NONE;
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
