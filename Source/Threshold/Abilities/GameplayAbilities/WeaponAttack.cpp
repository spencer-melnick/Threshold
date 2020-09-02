// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "WeaponAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Combat/Weapons/BaseWeapon.h"
#include "Threshold/Combat/Weapons/WeaponMoveset.h"


// Default constructor

UWeaponAttack::UWeaponAttack()
{
	bRetriggerInstancedAbility = true;
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

	// TODO: fix all of these repeated EndAbility calls D:
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const UWeaponMoveset* WeaponMoveset = GetMoveset(ActorInfo->AvatarActor.Get());

	if (!WeaponMoveset)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// TODO: Request the active weapon move from the client!
	const int32 NextWeaponMoveIndex = WeaponMoveset->GetNextWeaponMoveIndex(CurrentWeaponMoveIndex, EWeaponMoveType::Primary);
	const FWeaponMove* WeaponMove = WeaponMoveset->GetWeaponMove(NextWeaponMoveIndex);

	if (!WeaponMove || !WeaponMove->Animation)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Trigger the animation
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, WeaponMove->Animation);
	MontageTask->OnCompleted.AddDynamic(this, &UWeaponAttack::OnAnimationFinished);
	MontageTask->ReadyForActivation();

	// Keep track of where we are in our combo
	CurrentWeaponMoveIndex = NextWeaponMoveIndex;
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

	// Check that our next weapon move exists
	const UWeaponMoveset* WeaponMoveset = GetMoveset(ActorInfo->AvatarActor.Get());
	if (!WeaponMoveset)
	{
		return false;
	}
	
	return WeaponMoveset->IsValidMove(WeaponMoveset->GetNextWeaponMoveIndex(CurrentWeaponMoveIndex, EWeaponMoveType::Primary));
}




// Input buffering overrides

bool UWeaponAttack::CanBeRetriggered(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	check(ActorInfo);

	const ABaseCharacter* OwningCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	if (!OwningCharacter || !OwningCharacter->GetTHAbilitySystemComponent())
	{
		return false;
	}

	return OwningCharacter->GetTHAbilitySystemComponent()->HasMatchingGameplayTag(LocalComboTag);
}




// Helper functions

UWeaponMoveset* UWeaponAttack::GetMoveset(AActor* OwningActor)
{
	const ABaseCharacter* OwningCharacter = Cast<ABaseCharacter>(OwningActor);
	if (!OwningCharacter || !OwningCharacter->GetEquippedWeapon())
	{
		return nullptr;
	}

	return OwningCharacter->GetEquippedWeapon()->Moveset;
}




// Task callbacks

void UWeaponAttack::OnAnimationFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	// If the animation has ended then we didn't end up doing a combo, so our next move should start from the beginning
	UE_LOG(LogThresholdGeneral, Display, TEXT("Animation ended without doing a combo"))
	CurrentWeaponMoveIndex = -1;
}


