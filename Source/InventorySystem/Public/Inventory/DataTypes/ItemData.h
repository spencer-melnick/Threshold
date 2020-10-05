// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"

/**
 * Base class for item data. Override to add additional item data as needed.
 */
USTRUCT()
struct INVENTORYSYSTEM_API FInventoryItemDataBase
{
	GENERATED_BODY()

	virtual ~FInventoryItemDataBase() = default;

	/**
	 * Used for replication so that the inventory item knows the type of data to replicate.
	 * @return Struct type reflection data
	 */
	virtual UScriptStruct* GetScriptStruct() const { return StaticStruct(); }

	/**
	 * Create a copy of this data, allocated on the heap
	 */
	virtual FInventoryItemDataBase* Copy() const;
};
