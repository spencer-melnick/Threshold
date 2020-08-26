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
struct FBufferedAbilityInputData
{
	virtual ~FBufferedAbilityInputData() = default;
};
