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

	virtual bool IsValid() const { return true; }


	// Copying logic

	// Allocates a new copy of an inventory object - you will need to destroy this yourself!
	virtual FInventoryItem* Copy() const;

	
	
	// Network logic
	
	virtual UScriptStruct* GetScriptStruct() const { return StaticStruct(); }
	virtual bool operator==(const FInventoryItem& Other) const
	{
		return GetScriptStruct() && GetScriptStruct() == Other.GetScriptStruct();
	}
};



/**
 * Object that allows easy creation of custom inventory items in the editor
 */
UCLASS(Abstract, EditInlineNew)
class UInventoryItemHelper : public UObject
{
	GENERATED_BODY()

public:
	virtual FInventoryItem* GetItem() { return nullptr; }
};



/**
* Wrapper for inventory items that handles network serialization
*/
USTRUCT(BlueprintType)
struct FInventoryItemHandle : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryItemHandle() : ItemPointer(new FInventoryItem()) {};
	FInventoryItemHandle(FInventoryItem* Item) : ItemPointer(Item) {};
	FInventoryItemHandle(FInventoryItem& Item) : ItemPointer(Item.Copy()) {};
	FInventoryItemHandle(FInventoryItemHandle&& Other) : ItemPointer(MoveTemp(Other.ItemPointer)) {};
	FInventoryItemHandle(const FInventoryItemHandle& Other) : ItemPointer(Other.ItemPointer) {};

	FInventoryItemHandle& operator=(FInventoryItemHandle&& Other) { ItemPointer = MoveTemp(Other.ItemPointer); return *this; }
	FInventoryItemHandle& operator=(const FInventoryItemHandle& Other) { ItemPointer = Other.ItemPointer; return *this; }

	TSharedPtr<FInventoryItem> ItemPointer;

	TWeakPtr<FInventoryItem> Get() const { return ItemPointer; }
	FInventoryItem& operator*() const { return *ItemPointer; } 
	FInventoryItem* operator->() const { return ItemPointer.Get(); }
	bool operator==(const FInventoryItemHandle& Other) const { return ItemPointer.IsValid() && Other.ItemPointer.IsValid() && *ItemPointer == *Other.ItemPointer; }
	bool operator!=(const FInventoryItemHandle& Other) const { return !(FInventoryItemHandle::operator==(Other)); }

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool Serialize(FArchive& Ar);
};



/**
 * Simple example item
 */
USTRUCT(BlueprintType)
struct FSimpleInventoryItem : public FInventoryItem
{
	GENERATED_BODY()


	// Inventory item overrides

	virtual bool CanHaveDuplicates() const override { return bCanHaveDuplicates; }
	virtual bool CanStack() const override { return bCanStack; }
	virtual FGameplayTagContainer GetGameplayTags() override { return GameplayTags; }


	// Network overrides
	
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	virtual bool operator==(const FInventoryItem& Other) const override;
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);


	// Serialization

	bool Serialize(FArchive& Ar);
	

	
	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanHaveDuplicates = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanStack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GameplayTags;
};



/**
 * Helper for example item
 */
UCLASS(BlueprintType)
class USimpleInventoryItemHelper : public UInventoryItemHelper
{
	GENERATED_BODY()
	
public:
	virtual FInventoryItem* GetItem() override { return &Item; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSimpleInventoryItem Item;
};



// Enable network serialization on our structs
template<>
struct TStructOpsTypeTraits< FInventoryItemHandle > : public TStructOpsTypeTraitsBase2< FInventoryItemHandle >
{
	enum 
	{
		WithNetSerializer = true,
		WithSerializer = true,
	};
};

template<>
struct TStructOpsTypeTraits< FSimpleInventoryItem > : public TStructOpsTypeTraitsBase2< FSimpleInventoryItem >
{
	enum 
	{
		WithNetSerializer = true,
	};
};
