// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "THAbilitySystemComponent.generated.h"



UCLASS()
class UTHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTHAbilitySystemComponent();



	// Local gameplay cue functions
	
	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag,
		const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
    void AddGameplayCueLocal(const FGameplayTag GameplayCueTag,
    	const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
    void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
    	const FGameplayCueParameters& GameplayCueParameters);
};
