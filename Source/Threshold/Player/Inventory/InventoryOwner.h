// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InventoryOwner.generated.h"


// Forward declarations

class UInventoryComponent;


/**
* Interface used for any object containing an inventory component. Allows the inventory component to be stored elsewhere
* (such as in the player state but accessed from the character)
*/
UINTERFACE()
class UInventoryOwner : public UInterface
{
	GENERATED_BODY()
};

class IInventoryOwner
{
	GENERATED_BODY()

public:
	virtual UInventoryComponent* GetInventoryComponent() const = 0;
};

