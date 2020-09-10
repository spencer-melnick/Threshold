// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilityFunctionLibrary.generated.h"

/**
 * Simple function library for reusable ability functions, such as spawning an event/cue outside of a gameplay
 * effect
 */
UCLASS()
class UAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ability")
	static FGameplayEventData CreateGameplayEvent(AActor* Instigator, AActor* Target, FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category="Ability")
	static FGameplayCueParameters CreateGameplayCue(AActor* Instigator, AActor* EffectCauser, AActor* Target);
};
