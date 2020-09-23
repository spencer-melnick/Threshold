// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppSpecialFunctionWithoutNoexceptSpecification
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.generated.h"



/**
 * Base interface for all inventory items.
 */
USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	virtual ~FInventoryItem() = default;

	
	// Simple properties
	
	// True if owner of this item can have multiples of this item
	virtual bool CanHaveDuplicates() const { return true; }

	// True if this item can be stacked
	virtual bool CanStack() const { return false; }
	
	virtual FGameplayTagContainer GetGameplayTags() { return FGameplayTagContainer::EmptyContainer; }

	// Returns the class of the actor used to preview this item in the inventory viewport
	virtual TSoftClassPtr<AActor> GetPreviewActorClass() { return TSoftClassPtr<AActor>(); }


	
	// Network logic
	
	virtual UScriptStruct* GetScriptStruct() const { return nullptr; }

	virtual bool operator==(const FInventoryItem& Other) const
	{
		return GetScriptStruct() && GetScriptStruct() == Other.GetScriptStruct();
	}
};



/**
* Wrapper for inventory items that handles network serialization
*/
USTRUCT(BlueprintType)
struct FInventoryItemHandle : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryItemHandle() {};
	FInventoryItemHandle(FInventoryItem* Item) : ItemPointer(Item) {};
	FInventoryItemHandle(FInventoryItemHandle&& Other) : ItemPointer(MoveTemp(Other.ItemPointer)) {};
	FInventoryItemHandle(const FInventoryItemHandle& Other) : ItemPointer(Other.ItemPointer) {};

	FInventoryItemHandle& operator=(FInventoryItemHandle&& Other) { ItemPointer = MoveTemp(Other.ItemPointer); return *this; }
	FInventoryItemHandle& operator=(const FInventoryItemHandle& Other) { ItemPointer = Other.ItemPointer; return *this; }

	TSharedPtr<FInventoryItem> ItemPointer;

	TWeakPtr<FInventoryItem> Get() const { return ItemPointer; }
	FInventoryItem& operator->() const { return *ItemPointer; }
	bool operator==(const FInventoryItemHandle& Other) const { return ItemPointer.IsValid() && Other.ItemPointer.IsValid() && *ItemPointer == *Other.ItemPointer; }
	bool operator!=(const FInventoryItemHandle& Other) const { return !(FInventoryItemHandle::operator==(Other)); }

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};



/**
 * Simple example item
 */
USTRUCT(BlueprintType)
struct FSimpleUniqueInventoryItem : public FInventoryItem
{
	GENERATED_BODY()


	// Inventory item overrides

	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	virtual bool operator==(const FInventoryItem& Other) const override;
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	

	
	// Editor properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GameplayTags;
};


// Enable network serialization on our structs
template<>
struct TStructOpsTypeTraits< FInventoryItemHandle > : public TStructOpsTypeTraitsBase2< FInventoryItemHandle >
{
	enum 
	{
		WithNetSerializer = true,
   };
};

template<>
struct TStructOpsTypeTraits< FSimpleUniqueInventoryItem > : public TStructOpsTypeTraitsBase2< FSimpleUniqueInventoryItem >
{
	enum 
	{
		WithNetSerializer = true,
   };
};
