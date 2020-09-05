// Copyright (c) 2020 Spencer Melnick

#include "THGameplayAbility.h"
#include "THAbilitySystemComponent.h"
#include "Threshold/Character/Animation/AnimNotifyState_LooseTag.h"



UTHGameplayAbility::UTHGameplayAbility()
	: Super()
{
	// Set defaults
}

bool UTHGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	check(ActorInfo);
	
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (!AbilitySystemComponent)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	UAnimMontage* AnimMontage = AbilitySystemComponent->GetCurrentMontage();
	const bool bIsAuthority = ActorInfo->IsNetAuthority();
	const bool bIsLocallyControlled = ActorInfo->IsLocallyControlled();
	const bool bShouldLookForwardMontage = AnimMontage != nullptr && AnimInstance != nullptr && bIsAuthority && !bIsLocallyControlled;

	FGameplayTagContainer PredictedTags;
	
	if (bShouldLookForwardMontage)
	{
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

			// Apply our tags to the ability system component
			AbilitySystemComponent->AddLooseGameplayTags(AnimNotifyState->AppliedTags);
			PredictedTags.AppendTags(AnimNotifyState->AppliedTags);
		}
	}

	const bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bShouldLookForwardMontage)
	{
		// Remove our predicted tags
		AbilitySystemComponent->RemoveLooseGameplayTags(PredictedTags);
	}

	return Result;
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

