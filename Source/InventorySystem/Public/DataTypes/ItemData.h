// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"

/**
 * Base class for item data. Override to add additional item data as needed.
 */
USTRUCT()
struct FItemDataBase
{
	GENERATED_BODY()

	virtual ~FItemDataBase() = default;

	/**
	 * Used for replication so that the inventory item knows the type of data to replicate.
	 * @return Struct type reflection data
	 */
	virtual UScriptStruct* GetScriptStruct() const { return StaticStruct(); }
};
