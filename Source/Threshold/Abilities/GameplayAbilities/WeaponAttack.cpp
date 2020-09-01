// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "WeaponAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"


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

	UAbilityTask_WaitInputPress* InputTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	InputTask->OnPress.AddDynamic(this, &UWeaponAttack::OnInputPressed);
	InputTask->ReadyForActivation();
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

	check(ActorInfo);

	const ABaseCharacter* OwningCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	if (!OwningCharacter)
	{
		return false;
	}

	return (OwningCharacter->GetEquippedWeapon() != nullptr);
}




// Input buffering overrides

bool UWeaponAttack::GetCanAcceptInputPressed(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	check(ActorInfo);

	const ABaseCharacter* OwningCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	if (!OwningCharacter)
	{
		return false;
	}

	UTHAbilitySystemComponent* AbilitySystemComponent = OwningCharacter->GetTHAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		return false;
	}

	return AbilitySystemComponent->HasMatchingGameplayTag(LocalComboTag);
}





// Task callbacks

void UWeaponAttack::OnAnimationFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWeaponAttack::OnInputPressed(float ElapsedTime)
{
	// Check if we can trigger an attack combo
	UE_LOG(LogThresholdGeneral, Display, TEXT("Weapon input triggered"))
}


