// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.generated.h"



UENUM()
enum class EInventoryStorageBehavior : uint8
{
	Unique,			// Only one of this item can be present in the player's inventory at a time
	
	Duplicate,		// Multiples of this item can be present in the player's inventory, but they will not appear as a stack
					// Useful for items that the player can have multiples of, but may have unique properties, such as
					// different stat bonuses, effect amounts, etc.

	Stack,			// Multiples of this item will be added to the inventory as a stack.
					// Useful for generic items that the player will have a lot of (crafting components, simple potions, etc.)

	StackUnique		// Same as stack, but only one stack can be present in the player's inventory
};



/**
 * Base interface for any item that can be stored in the player's inventory. This is not the physical representation
 * of the inventory item (it should not have any components, nor should it be an actor), but the data only.
 * Additionally, no data should be stored as the inventory items themselves will not be replicated, only the CDOs
 * (defaults can be edited in Blueprints, as these will be saved in the CDOs)
 */
UINTERFACE()
class UInventoryItem : public UInterface
{
	GENERATED_BODY()
};

class IInventoryItem
{
	GENERATED_BODY()

public:
	virtual EInventoryStorageBehavior GetStorageBehavior() const = 0;

	virtual int32 GetMaxStackSize() const { return -1; }

	virtual FGameplayTagContainer GetGameplayTags() const = 0;

	// Returns the class of the actor used to preview this item in the inventory viewport
	virtual TSoftClassPtr<AActor> GetPreviewActorClass() const = 0;
};
