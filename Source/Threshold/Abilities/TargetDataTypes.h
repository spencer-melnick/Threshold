// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "TargetDataTypes.generated.h"

USTRUCT()
struct FAbilityDirectionalData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	FVector Direction;
};
