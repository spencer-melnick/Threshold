// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "WeaponAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"




// Default constructor

UWeaponAttack::UWeaponAttack()
{
	DefaultInputBinding = EAbilityInputType::PrimaryAttack;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}




// Engine overrides

void UWeaponAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UWeaponAttack::OnAnimationFinished);
	MontageTask->ReadyForActivation();
}

bool UWeaponAttack::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Optional activation logic
	return true;
}




// Task callbacks

void UWeaponAttack::OnAnimationFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

