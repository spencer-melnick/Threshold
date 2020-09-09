// Copyright (c) 2020 Spencer Melnick

#include "WeaponAttack.h"
#include "Threshold/Abilities/Tasks/AT_PlayMontageAndWaitForEvent.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Combat/Weapons/BaseWeapon.h"


// Default constructor

UWeaponAttack::UWeaponAttack()
{
	DefaultInputBinding = EAbilityInputType::PrimaryAttack;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
		return;
	}

	// Trigger the animation
	UAT_PlayMontageAndWaitForEvent* MontageTask =
        UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontage, HitEventTags);
	MontageTask->OnCompleted.AddDynamic(this, &UWeaponAttack::OnAnimationFinished);
	MontageTask->EventReceived.AddDynamic(this, &UWeaponAttack::OnEventReceived);
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

	return true;
}



// Task callbacks

void UWeaponAttack::OnAnimationFinished(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWeaponAttack::OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (HasAuthority(&CurrentActivationInfo) && DamageEffect)
	{
		// Only apply damage on the server
		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(BaseDamageTag, BaseDamageAmount);
		
		// ReSharper disable once CppExpressionWithoutSideEffects
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DamageEffectSpecHandle, EventData.TargetData);
	}
}

