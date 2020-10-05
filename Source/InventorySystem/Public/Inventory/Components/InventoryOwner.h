// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InventoryOwner.generated.h"


// Forward declarations

class UInventoryComponent;
struct FInventoryItem;


UINTERFACE()
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
	virtual UInventoryComponent* GetInventoryComponent() const = 0;

	/**
	* Try to add an inventory item to this inventory.
	* @return The count of the items added on success or 0 on failure
	*/
	int32 AddItem(const FInventoryItem& NewItem);
};
