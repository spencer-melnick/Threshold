// Copyright © 2020 Spencer Melnick

#include "CharacterDodge.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GameFramework/Character.h"
#include "Threshold/Abilities/Tasks/AbilityTask_ApplyRootMotionPositionCurve.h"


UCharacterDodge::UCharacterDodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Dodge"), false));

	DefaultInputBinding = EAbilityInputType::Dodge;
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

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	const FVector Direction = Character->GetLastMovementInputVector().GetSafeNormal();

	// Run a root motion task to apply dodge motion
	UAbilityTask_ApplyRootMotionPositionCurve* RootMotionTask =
    UAbilityTask_ApplyRootMotionPositionCurve::ApplyRootMotionPositionCurve(this, NAME_None,
        Direction, DodgeDistance, DodgeDuration, PositionCurve);
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

