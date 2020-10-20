// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InventoryOwner.generated.h"


// Forward declarations

class UInventoryComponent;
struct FInventoryItem;
struct FInventoryArrayHandle;


UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class INVENTORYSYSTEM_API UInventoryOwner : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to be implemented by any object holding an inventory component for easier access
 */
class INVENTORYSYSTEM_API IInventoryOwner
{
	GENERATED_BODY()

public:

	// Type aliases

	using FAdditionResult = TPair<int32, FInventoryArrayHandle>;


	// Interface functions
	
	UFUNCTION(BlueprintCallable, Category=InventoryOwner)
	virtual UInventoryComponent* GetInventoryComponent() const = 0;

	/**
	* Try to add an inventory item to this inventory.
	* @return The count of the items added on success or 0 on failure, and a handle to the item added (if any)
	*/
	FAdditionResult AddItem(const FInventoryItem& NewItem);
};
