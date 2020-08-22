// Copyright © 2020 Spencer Melnick

#include "CharacterDodge.h"
#include "GameFramework/Character.h"
#include "Threshold/Abilities/Tasks/AbilityTask_ApplyRootMotionPositionCurve.h"
#include "Threshold/Abilities/Tasks/AT_ServerWaitForClientTargetData.h"
#include "Threshold/Abilities/AbilityInputTypes.h"
#include "Threshold/Abilities/TargetDataTypes.h"


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

	if (IsLocallyControlled())
	{
		// If this is running on the client calculate direction
		const FVector Direction = Character->GetLastMovementInputVector().GetSafeNormal();

		if (IsPredictingClient())
		{
			// Send the directional data to the server
			FAbilityDirectionalData* DirectionalData = new FAbilityDirectionalData();
			DirectionalData->Direction = Direction;
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(DirectionalData);
			SendTargetDataToServer(TargetDataHandle);
		}
		
		// Apply the locally simulated motion
		ApplyDodgeMotionTask(Direction);
	}
	else
	{
		// Otherwise wait for the directional data from the client
		UAT_ServerWaitForClientTargetData* WaitTask =
			UAT_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, true);
		WaitTask->ValidData.AddDynamic(this, &UCharacterDodge::OnClientDataReceived);
		WaitTask->ReadyForActivation();
	}
	
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

void UCharacterDodge::ApplyDodgeMotionTask(const FVector Direction)
{
	// Run a root motion task to apply dodge motion
	UAbilityTask_ApplyRootMotionPositionCurve* RootMotionTask =
    UAbilityTask_ApplyRootMotionPositionCurve::ApplyRootMotionPositionCurve(this, NAME_None,
        Direction, DodgeDistance, DodgeDuration, PositionCurve);
	RootMotionTask->OnFinish.AddDynamic(this, &UCharacterDodge::OnDodgeFinished);
	RootMotionTask->ReadyForActivation();
}

void UCharacterDodge::OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!Data.IsValid(0) || Data.Num() != 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterDodge received wrong amount of target data from client; expected 1 got %d"), Data.Num());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = Data.Get(0);

	if (TargetData->GetScriptStruct() != FAbilityDirectionalData::StaticStruct())
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterDodge received incorrect data type from client"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const FAbilityDirectionalData* DirectionalData = static_cast<const FAbilityDirectionalData*>(TargetData);
	ApplyDodgeMotionTask(DirectionalData->Direction);
}


void UCharacterDodge::OnDodgeFinished()
{
	check(CurrentActorInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

