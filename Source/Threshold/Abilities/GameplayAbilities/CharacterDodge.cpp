// Copyright (c) 2020 Spencer Melnick

#include "CharacterDodge.h"
#include "GameFramework/Character.h"
#include "Threshold/Threshold.h"
#include "Threshold/Abilities/Tasks/AbilityTask_ApplyRootMotionPositionCurve.h"
#include "Threshold/Abilities/Tasks/AT_ServerWaitForClientTargetData.h"
#include "Threshold/Abilities/AbilityInputTypes.h"
#include "Threshold/Abilities/TargetDataTypes.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"




// Default constructor

UCharacterDodge::UCharacterDodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Dodge"), false));

	DefaultInputBinding = EAbilityInputType::Dodge;
}




// Engine overrides

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

	const UTHAbilitySystemComponent* AbilitySystemComponent =
		Cast<UTHAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !AbilitySystemComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (IsLocallyControlled() || IsPredictingClient())
	{
		// Try to grab our input data
		const TSharedPtr<FDodgeInputData> InputData =
            AbilitySystemComponent->GetPendingAbilityInput<FDodgeInputData>().Pin();

		if (!InputData.IsValid())
		{
			UE_LOG(LogThresholdGeneral, Error, TEXT("%s was called with incorrect input data type"), *GetNameSafe(this))
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		
		// Send the directional data to the server
		FAbilityDirectionalData* DirectionalData = new FAbilityDirectionalData();
		DirectionalData->Direction = InputData->DodgeVector;
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(DirectionalData);
		SendTargetDataToServer(TargetDataHandle);
	
		// Apply the locally simulated motion
		ApplyDodgeMotionTask(InputData->DodgeVector);
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

// THGameplayAbility overrides

TSharedPtr<FBufferedAbilityInputData> UCharacterDodge::GenerateInputData(const FGameplayAbilitySpecHandle SpecHandle,
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		return nullptr;
	}

	TSharedPtr<FDodgeInputData> InputData = MakeShared<FDodgeInputData>();
	const FVector LastMovementVector = Character->GetLastMovementInputVector();

	if (LastMovementVector.IsNearlyZero())
	{
		// If our movement vector is nearly zero, dodge backwards from the control (i.e. camera) rotation
		InputData->DodgeVector = Character->GetControlRotation().RotateVector(FVector::BackwardVector);
	}
	else
	{
		// Otherwise dodge in the direction of the movement
		InputData->DodgeVector = LastMovementVector.GetSafeNormal();
	}

	return MoveTemp(InputData);
}




// Helper functions

void UCharacterDodge::ApplyDodgeMotionTask(const FVector Direction)
{
	
	// Run a root motion task to apply dodge motion
	UAbilityTask_ApplyRootMotionPositionCurve* RootMotionTask =
    UAbilityTask_ApplyRootMotionPositionCurve::ApplyRootMotionPositionCurve(this,
    	NAME_None, Direction, DodgeDistance, DodgeDuration, PositionCurve, LocalDodgeTag);
	RootMotionTask->OnFinish.AddDynamic(this, &UCharacterDodge::OnDodgeFinished);
	RootMotionTask->ReadyForActivation();
}

void UCharacterDodge::OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!Data.IsValid(0) || Data.Num() != 1)
	{
		// Check to see that we actually got the right amount of valid data
		UE_LOG(LogTemp, Error, TEXT("UCharacterDodge received wrong amount of target data from client; expected 1 got %d"), Data.Num());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = Data.Get(0);

	if (TargetData->GetScriptStruct() != FAbilityDirectionalData::StaticStruct())
	{
		// Check if the data type is actually correct before attempting to cast
		UE_LOG(LogTemp, Error, TEXT("UCharacterDodge received incorrect data type from client"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const FAbilityDirectionalData* DirectionalData = static_cast<const FAbilityDirectionalData*>(TargetData);
	ApplyDodgeMotionTask(DirectionalData->Direction);
}

void UCharacterDodge::OnDodgeFinished()
{
	check(CurrentActorInfo)

	// End this ability so we can reactivate it later
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

