// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ThresholdGame/Abilities/AbilityInputTypes.h"
#include "THGameplayAbility.generated.h"



UCLASS(Abstract)
class THRESHOLDGAME_API UTHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTHGameplayAbility();


	// Engine overrides

	virtual bool DoesAbilitySatisfyTagRequirements(
		const UAbilitySystemComponent& AbilitySystemComponent,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;


	// Input buffering code

	// This function is called before an ability with input buffering enabled is activated. It tells the ability
	// that it should use the PlayerController or Character state to generate some input that it should consume later.
	virtual TSharedPtr<FBufferedAbilityInputData> GenerateInputData(const FGameplayAbilitySpecHandle SpecHandle,
		const FGameplayAbilityActorInfo* ActorInfo)
	{
		return nullptr;
	}

	// Only instanced abilities with remote activation disabled can be buffered with custom input data currently,
	// due to the mechanism for storing and sending input data not traveling through normal GAS paths
	virtual bool GetInputBufferingEnabled() const { return false; }

	// Public properties
	
	UPROPERTY(EditDefaultsOnly)
	EAbilityInputType DefaultInputBinding = EAbilityInputType::None;

protected:

	UFUNCTION(BlueprintCallable, Category="Gameplay Ability|Threshold")
	void SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData);
};
