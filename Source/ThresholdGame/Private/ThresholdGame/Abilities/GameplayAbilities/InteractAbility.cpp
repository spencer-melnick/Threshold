﻿// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/GameplayAbilities/InteractAbility.h"

#include "ThresholdGame.h"
#include "ThresholdGame/Abilities/TargetDataTypes.h"
#include "ThresholdGame/Abilities/THAbilitySystemComponent.h"
#include "ThresholdGame/Abilities/Tasks/AT_ServerWaitForClientTargetData.h"
#include "ThresholdGame/Abilities/AbilityFunctionLibrary.h"
#include "ThresholdGame/Player/THPlayerController.h"
#include "ThresholdGame/Character/BaseCharacter.h"
#include "ThresholdGame/World/InteractiveObject.h"


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

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
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
		const TWeakInterfacePtr<IInteractiveObject> TargetObject = GetTargetObject(ActorInfo);

		if (!TargetObject.IsValid())
		{
			// Cancel this ability if we somehow didn't have a valid target object
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		if (IsPredictingClient())
		{
			// Send target object data on the predicting client
			SendTargetDataToServer(new FSingleObjectTargetData(TargetObject.GetObject()));
		}
		else
		{
			// On the listen server's player, just trigger the interaction
			TriggerInteraction(TargetObject);
		}
	}
}

bool UInteractAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (ActorInfo->IsLocallyControlled())
	{
		// For owner check if there is a target object before triggering interaction
		return GetTargetObject(ActorInfo).IsValid();
	}
	else
	{
		// On the server, assume we can interact and check the target later
		return true;
	}
}


void UInteractAbility::OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	const FSingleObjectTargetData* ObjectData = UAbilityFunctionLibrary::ConvertTargetData<FSingleObjectTargetData>(Data);
	if (!ObjectData || !ObjectData->Object.IsValid() || !ObjectData->Object->Implements<UInteractiveObject>())
	{
		UE_LOG(LogThresholdGame, Error, TEXT("UInteractAbility received an invalid target object"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const TWeakInterfacePtr<IInteractiveObject> InteractiveObject(*Cast<IInteractiveObject>(ObjectData->Object));

	// Check if the object is in a reasonable range from the character
	if (!CheckInteractionRange(InteractiveObject))
	{
		if (CurrentActorInfo)
		{
			UE_LOG(LogThresholdGame, Warning, TEXT("UInteractAbility received a target object out of range from %s"),
				*GetNameSafe(CurrentActorInfo->PlayerController.Get()));
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	// Trigger the interaction
	TriggerInteraction(InteractiveObject);
}

TWeakInterfacePtr<IInteractiveObject> UInteractAbility::GetTargetObject(const FGameplayAbilityActorInfo* ActorInfo) const
{
	// Try to get the current object from the player controller
	const ATHPlayerController* PlayerController = UAbilityFunctionLibrary::GetPlayerControllerFromActorInfo<ATHPlayerController>(CurrentActorInfo);
	if (PlayerController)
	{			
		return PlayerController->GetCurrentInteractiveObject();
	}
	return TWeakInterfacePtr<IInteractiveObject>();
}

bool UInteractAbility::CheckInteractionRange(const TWeakInterfacePtr<IInteractiveObject>& InteractiveObject) const
{
	const ABaseCharacter* BaseCharacter = UAbilityFunctionLibrary::GetCharacterFromActorInfo<ABaseCharacter>(CurrentActorInfo);
	const ATHPlayerController* PlayerController = UAbilityFunctionLibrary::GetPlayerControllerFromActorInfo<ATHPlayerController>(CurrentActorInfo);

	if (!BaseCharacter || !PlayerController)
	{
		return false;
	}

	// Use the squared range to speed up computation a little
	const float StartingRange = PlayerController->MaxInteractionDistance;
	const float AdditionalRange = IConsoleManager::Get().FindConsoleVariable(TEXT("th.AdditionalInteractionRange"))->GetFloat();
	const float SquaredRange = FMath::Square(StartingRange + AdditionalRange);

    return (FVector::DistSquared(BaseCharacter->GetActorLocation(), InteractiveObject->GetInteractLocation()) < SquaredRange);
}



void UInteractAbility::TriggerInteraction(TWeakInterfacePtr<IInteractiveObject> Object)
{
	ABaseCharacter* BaseCharacter = UAbilityFunctionLibrary::GetCharacterFromActorInfo<ABaseCharacter>(CurrentActorInfo);

	if (!Object.IsValid() || !Object->CanInteract(BaseCharacter))
	{
		UE_LOG(LogThresholdGame, Error, TEXT("UInteractAbility tried to trigger an interaction with an invalid interactive object"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Object->OnServerInteract(BaseCharacter);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

