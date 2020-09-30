// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "StackData.generated.h"


// Forward declarations


/**
 * Simple data type that holds a stack count
 */
USTRUCT()
struct INVENTORYSYSTEM_API FInventoryStackData : public FInventoryItemDataBase
{
	GENERATED_BODY()

	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }

	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;
};
