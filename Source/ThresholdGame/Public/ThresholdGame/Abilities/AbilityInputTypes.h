// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilityInputTypes.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputType : uint8
{
	None,
	Dodge,
	Confirm,
	Cancel,
	Jump,
	PrimaryAttack,
	SecondaryAttack,
	Interact
};

// This is the base struct for any input data we're feeding to buffered abilities
USTRUCT()
struct THRESHOLDGAME_API FBufferedAbilityInputData
{
	GENERATED_BODY()
	
	virtual ~FBufferedAbilityInputData() = default;

	virtual UScriptStruct* GetScriptStruct() const
	{
		return FBufferedAbilityInputData::StaticStruct();
	}
};
