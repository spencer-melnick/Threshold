// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemTypes/ItemType.h"
#include "StackItemType.generated.h"



// Forward declarations

struct FInventoryStackData;



/**
 * Simple inventory item type that allows for the storage of stacks of items
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryStackItemType : public UInventoryItemType
{
	GENERATED_BODY()

public:

	// Item type overrides

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
	virtual UScriptStruct* GetItemDataType() const override;
	virtual TSharedPtr<FInventoryItemDataBase> CreateItemData() const override;
	virtual bool AllowsStacking() const override { return true; }
	virtual int32 AddToStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const override;
	virtual int32 RemoveFromStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const override;
	virtual void SetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const override;
	virtual int32 GetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData) const override;


	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemType)
	int32 MaxStackSize = 99;
};


/**
 * Utility class that contains some useful functions for implementing stackable items
 */
class INVENTORYSYSTEM_API FStackItemTypeImplementation
{
public:
	/**
	 * Increments the stack by the set count, limited by stack size
	 * @return Count actually added to stack after applying limits
	 */
	static int32 AddToStack(int32& Stack, const int32 Count, const int32 MaxStackSize);

	/**
	 * Decrements the stack by the set count, limited by 0
	 * @return Count actually removed from stack after applying limits
	 */
	static int32 RemoveFromStack(int32& Stack, const int32 Count);
};
