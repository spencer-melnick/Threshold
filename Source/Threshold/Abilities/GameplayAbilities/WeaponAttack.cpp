// Copyright (c) 2020 Spencer Melnick

#include "WeaponAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Threshold/Abilities/Tasks/AT_ServerWaitForClientTargetData.h"
#include "Threshold/Threshold.h"
#include "Threshold/Abilities/TargetDataTypes.h"
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

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (IsLocallyControlled() || IsPredictingClient())
		{
			// Only do checks and moveset calculation if we're the listen server or the client
			
			check(ActorInfo);
			const UWeaponMoveset* Moveset = GetMoveset(ActorInfo->AvatarActor.Get());

			if (Moveset)
			{
				const int32 NextWeaponMoveIndex = Moveset->GetNextWeaponMoveIndex(CurrentWeaponMoveIndex, EWeaponMoveType::Primary);
				if (Moveset->IsValidMove(NextWeaponMoveIndex))
				{
					if (IsPredictingClient())
					{
						// Put our move index into target data to send to the server
						FIntegralTargetData* MoveIndexTargetData = new FIntegralTargetData();
						MoveIndexTargetData->IntegralData = NextWeaponMoveIndex;
						FGameplayAbilityTargetDataHandle TargetDataHandle;
						TargetDataHandle.Add(MoveIndexTargetData);
						SendTargetDataToServer(TargetDataHandle);
					}

					if (PlayAttackAnimation(NextWeaponMoveIndex))
					{
						// Early exit on success so we skip the EndAbility call
						return;
					}
				}
			}
		}
		else
		{
			// If we're the server wait for the requested move from the client
			UAT_ServerWaitForClientTargetData* WaitTask =
				UAT_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, true);
			WaitTask->ValidData.AddDynamic(this, &UWeaponAttack::OnServerReceiveData);
			WaitTask->ReadyForActivation();
			return;
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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

bool UWeaponAttack::PlayAttackAnimation(int32 WeaponMoveIndex)
{
	const UWeaponMoveset* WeaponMoveset = GetMoveset(CurrentActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();

	if (WeaponMoveset && AbilitySystemComponent)
	{
		const FWeaponMove* WeaponMove = WeaponMoveset->GetWeaponMove(WeaponMoveIndex);

		if (WeaponMove && WeaponMove->Animation)
		{
			// Trigger the animation
			UAbilityTask_PlayMontageAndWait* MontageTask =
                UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, WeaponMove->Animation);
			MontageTask->OnCompleted.AddDynamic(this, &UWeaponAttack::OnAnimationFinished);
			MontageTask->ReadyForActivation();

			// Keep track of where we are in our combo
			CurrentWeaponMoveIndex = WeaponMoveIndex;

			return true;
		}
	}

	return false;
}





// Task callbacks

void UWeaponAttack::OnAnimationFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	// If the animation has ended then we didn't end up doing a combo, so our next move should start from the beginning
	CurrentWeaponMoveIndex = -1;
}

void UWeaponAttack::OnServerReceiveData(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!Data.IsValid(0) || Data.Num() != 1)
	{
		// Check to see that we actually got the right amount of valid data
		UE_LOG(LogThresholdGeneral, Error, TEXT("UWeaponAttack received wrong amount of target data from client; expected 1 got %d"), Data.Num());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = Data.Get(0);

	if (TargetData->GetScriptStruct() != FIntegralTargetData::StaticStruct())
	{
		// Check if the data type is actually correct before attempting to cast
		UE_LOG(LogThresholdGeneral, Error, TEXT("UWeaponAttack received incorrect data type from client"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const FIntegralTargetData* IntegralData = static_cast<const FIntegralTargetData*>(TargetData);
	// TODO: Validate the move based on rough timing here

	if (!PlayAttackAnimation(IntegralData->IntegralData))
	{
		UE_LOG(LogThresholdGeneral, Error, TEXT("UWeaponAttack received data on server, but could not perform the attack"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}


