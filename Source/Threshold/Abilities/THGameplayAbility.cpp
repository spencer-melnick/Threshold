// Copyright © 2020 Spencer Melnick

#include "THGameplayAbility.h"
#include "THAbilitySystemComponent.h"



UTHGameplayAbility::UTHGameplayAbility()
	: Super()
{
	// Set defaults
}

void UTHGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (!IsPredictingClient())
	{
		return;
	}

	UTHAbilitySystemComponent* AbilitySystemComponent =
		Cast<UTHAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);

	check(AbilitySystemComponent);
	
	FScopedPredictionWindow PredictionWindow(AbilitySystemComponent);
	AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle,
		CurrentActivationInfo.GetActivationPredictionKey(), TargetData, FGameplayTag::EmptyTag,
		PredictionWindow.ScopedPredictionKey);
}

