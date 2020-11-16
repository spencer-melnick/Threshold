// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/THGameplayAbility.h"
#include "ThresholdGame/Abilities/THAbilitySystemComponent.h"
#include "ThresholdGame/Character/Animation/AnimNotifyState_LooseTag.h"
#include "AbilitySystemGlobals.h"



UTHGameplayAbility::UTHGameplayAbility()
	: Super()
{
	// Set defaults
}

bool UTHGameplayAbility::DoesAbilitySatisfyTagRequirements(
	const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayTagContainer RelevantTags;
	if (!OptionalRelevantTags)
	{
		OptionalRelevantTags = &RelevantTags;
	}
	
	const bool bResult = Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);
	
	if (bResult)
	{
		// If we succeeded, don't bother checking anything! No such thing as false positive here
		return true;
	}

	const FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent.AbilityActorInfo.Get();
	check(ActorInfo);
	const bool bServerPlayback = !ActorInfo->IsLocallyControlled() && ActorInfo->IsNetAuthority();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	UAnimMontage* AnimMontage = AbilitySystemComponent.GetCurrentMontage();
	
	if (!bServerPlayback || !AnimInstance || !AnimMontage)
	{
		// Skip prediction if we're not replaying a client's actions, or the animation data isn't valid
		return bResult;
	}

	// Look ahead for activation tags if that's the only reason activation failed
	const UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	if (OptionalRelevantTags->Num() == 1 && OptionalRelevantTags->HasTagExact(AbilitySystemGlobals.ActivateFailTagsMissingTag))
	{
		// Start with all of our current tags
		FGameplayTagContainer PredictedTags;
		AbilitySystemComponent.GetOwnedGameplayTags(PredictedTags);
	
		// Look for future anim notifies based on the cvar time
		const float CurrentMontageTime = AnimInstance->Montage_GetPosition(AnimMontage);
		const float LookForwardTime = IConsoleManager::Get().FindConsoleVariable(TEXT("th.TagLookForwardTime"))->GetFloat();
		TArray<FAnimNotifyEventReference> NotifyReferences;
		AnimMontage->GetAnimNotifies(CurrentMontageTime, LookForwardTime, false, NotifyReferences);

		for (FAnimNotifyEventReference NotifyReference : NotifyReferences)
		{
			UAnimNotifyState_LooseTag* AnimNotifyState = Cast<UAnimNotifyState_LooseTag>(NotifyReference.GetNotify()->NotifyStateClass);
			if (!AnimNotifyState)
			{
				continue;
			}
			PredictedTags.AppendTags(AnimNotifyState->AppliedTags);
		}

		if (PredictedTags.HasAll(ActivationRequiredTags))
		{
			// If our predicted tags fill in all missing tags, ability activation should proceed
			return true;
		}
	}

	return bResult;
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

