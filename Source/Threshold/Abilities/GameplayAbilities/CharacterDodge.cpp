// Copyright © 2020 Spencer Melnick

#include "CharacterDodge.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Threshold/Abilities/TargetDataTypes.h"
#include "Threshold/Abilities/Tasks/AbilityTask_ApplyRootMotionPositionCurve.h"


UCharacterDodge::UCharacterDodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Dodge"), false));

	DefaultInputBinding = EAbilityInputType::Dodge;
	bRequiresDirectionInput = true;
}

void UCharacterDodge::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	// Try to pull our directional data from the event data
	const FGameplayAbilityTargetDataHandle& TargetData = TriggerEventData->TargetData;
	checkf(TargetData.Num() > 0, TEXT("UCharacterDodge called with no target data"));
	const FAbilityDirectionalData* DirectionalData = static_cast<const FAbilityDirectionalData*>(TargetData.Get(0));

	// Run a root motion task to apply dodge motion
	UAbilityTask_ApplyRootMotionPositionCurve* RootMotionTask =
    UAbilityTask_ApplyRootMotionPositionCurve::ApplyRootMotionPositionCurve(this, NAME_None,
        DirectionalData->Direction, DodgeDistance, DodgeDuration, PositionCurve);
	RootMotionTask->OnFinish.AddDynamic(this, &UCharacterDodge::OnDodgeFinished);
	RootMotionTask->ReadyForActivation();
}

bool UCharacterDodge::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UCharacterDodge::OnDodgeFinished()
{
	check(CurrentActorInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

