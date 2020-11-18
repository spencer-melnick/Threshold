// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/GameplayAbilities/HitSlowdown.h"
#include "ThresholdGame/Abilities/Tasks/AT_ApplySlowdownCurve.h"
#include "AbilitySystemComponent.h"


// UHitSlowdown

UHitSlowdown::UHitSlowdown()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bRetriggerInstancedAbility = true;
}



// Gameplay ability overrides

void UHitSlowdown::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !AbilitySystemComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAT_ApplySlowdownCurve* SlowdownTask = UAT_ApplySlowdownCurve::ApplySlowdownCurve(
		this, NAME_None, AbilitySystemComponent->GetCurrentMontage(), SlowdownCurve, Duration);
	SlowdownTask->OnEnd.AddDynamic(this, &UHitSlowdown::OnSlowdownEnded);
	SlowdownTask->ReadyForActivation();
}



// Task delegates

void UHitSlowdown::OnSlowdownEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


