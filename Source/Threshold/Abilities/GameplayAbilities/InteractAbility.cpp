// Copyright (c) 2020 Spencer Melnick

#include "InteractAbility.h"

#include "Threshold/Threshold.h"
#include "Threshold/Abilities/TargetDataTypes.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"
#include "Threshold/Abilities/Tasks/AT_ServerWaitForClientTargetData.h"
#include "Threshold/Controllers/THPlayerController.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/World/InteractiveObject.h"


UInteractAbility::UInteractAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	DefaultInputBinding = EAbilityInputType::Interact;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UInteractAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	const UTHAbilitySystemComponent* AbilitySystemComponent =
        Cast<UTHAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !AbilitySystemComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsForRemoteClient())
	{
		// Wait for the target data on the server
		
		UAT_ServerWaitForClientTargetData* DataTask = UAT_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, true);
		DataTask->ValidData.AddDynamic(this, &UInteractAbility::OnClientDataReceived);
		DataTask->ReadyForActivation();
	}
	else
	{
		// Try to get the current object from the player controller
		TWeakInterfacePtr<IInteractiveObject> TargetObject;
		const ATHPlayerController* PlayerController = Cast<ATHPlayerController>(ActorInfo->PlayerController);
		if (PlayerController)
		{			
			TargetObject = PlayerController->GetCurrentInteractiveObject();
		}

		if (!TargetObject.IsValid())
		{
			// Cancel this ability if we somehow didn't have a valid target object
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		if (IsPredictingClient())
		{
			// Send target object data on the predicting client
			
			FSingleObjectTargetData* ObjectData = new FSingleObjectTargetData();
			ObjectData->Object = TargetObject.GetObject();
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(ObjectData);
			SendTargetDataToServer(TargetDataHandle);
		}
		else
		{
			// On the listen server's player, just trigger the interaction
			TriggerInteraction(TargetObject);
		}
	}
}

void UInteractAbility::OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	// TODO: Add simple function for converting a generic target data handle into the target data with checks
	// TODO: Check that interactive object is within a reasonable range
	
	if (!Data.IsValid(0) || Data.Num() != 1)
	{
		// Check to see that we actually got the right amount of valid data
		UE_LOG(LogThresholdGeneral, Error, TEXT("UInteractAbility received wrong amount of target data from client; expected 1 got %d"), Data.Num());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = Data.Get(0);

	if (TargetData->GetScriptStruct() != FSingleObjectTargetData::StaticStruct())
	{
		// Check if the data type is actually correct before attempting to cast
		UE_LOG(LogThresholdGeneral, Error, TEXT("UInteractAbility received incorrect data type from client"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const FSingleObjectTargetData* ObjectData = static_cast<const FSingleObjectTargetData*>(TargetData);
	if (!ObjectData->Object.IsValid() || !ObjectData->Object->Implements<UInteractiveObject>())
	{
		UE_LOG(LogThresholdGeneral, Error, TEXT("UInteractAbility received an invalid target object"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// Trigger the actual interaction
	const TWeakInterfacePtr<IInteractiveObject> InteractiveObject(*Cast<IInteractiveObject>(ObjectData->Object));
	TriggerInteraction(InteractiveObject);
}

void UInteractAbility::TriggerInteraction(TWeakInterfacePtr<IInteractiveObject> Object)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(CurrentActorInfo->AvatarActor);

	if (!Object.IsValid() || !Object->CanInteract(BaseCharacter))
	{
		UE_LOG(LogThresholdGeneral, Error, TEXT("UInteractAbility tried to trigger an interaction with an invalid interactive object"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Object->OnServerInteract(BaseCharacter);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

