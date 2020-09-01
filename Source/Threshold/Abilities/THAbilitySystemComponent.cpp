// Copyright (c) 2020 Spencer Melnick

#include "THAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "THGameplayAbility.h"
#include "GameplayCueManager.h"
#include "Threshold/Threshold.h"


// Default constructor

UTHAbilitySystemComponent::UTHAbilitySystemComponent()
{

}




// Constants

const int32 UTHAbilitySystemComponent::MaxInputBufferSize = 2;




// Engine overrides

bool UTHAbilitySystemComponent::GetShouldTick() const
{
	if (bEnableInputBuffering && !InputBuffer.IsEmpty())
	{
		// We need to tick if there is some input in the queue
		return true;
	}

	// Otherwise fall back to the default tick status
	return Super::GetShouldTick();
}




// The following function is a modified version of the code present in the Unreal Engine source, the original code is
// Copyright Epic Games, Inc. All Rights Reserved.
void UTHAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID != InputID || !Spec.Ability)
		{
			continue;
		}
		
		UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);
		Spec.InputPressed = true;
		
		if (Spec.IsActive())
		{
			if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
			{
				ServerSetInputPressed(Spec.Handle);
			}

			AbilitySpecInputPressed(Spec);

			// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
		}

		if (GameplayAbility)
		{
			if (!GameplayAbility->GetInputBufferingEnabled() && !Spec.IsActive())
			{
				// Activate non-buffered abilities normally
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				// Generate some input data
				TSharedPtr<FBufferedAbilityInputData> InputData =
					GameplayAbility->GenerateInputData(Spec.Handle, AbilityActorInfo.Get());

				if (!Spec.IsActive() && GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get()))
				{
					// If we can activate the ability make the data available and activate the ability normally
					MostRecentInputData = InputData;
					TryActivateAbility(Spec.Handle);
				}
				else if (bEnableInputBuffering && GameplayAbility->GetInputBufferingEnabled())
				{
					BufferInput({InputID, InputData, GetWorld()->GetRealTimeSeconds()});
				}
			}
		}
	}
}

void UTHAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bEnableInputBuffering || InputBuffer.IsEmpty())
	{
		return;
	}

	bool bFoundBlockedInput = false;

	while (!InputBuffer.IsEmpty() && !bFoundBlockedInput)
	{			
		FBufferedInput* Input = InputBuffer.Peek();

		if (GetWorld()->GetRealTimeSeconds() - Input->InputTime > InputBufferingTime)
		{
			// Remove expired inputs and check the next input
			RemoveFrontInput();
			continue;
		}
		
		ABILITYLIST_SCOPE_LOCK();
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			// Mostly follow the normal input logic here
			if (Spec.InputID != Input->InputID || !Spec.Ability)
			{
				continue;	
			}
			
			Spec.InputPressed = true;
			if (!Spec.IsActive())
			{
				UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);
				
				if (!GameplayAbility || !GameplayAbility->GetInputBufferingEnabled())
				{
					// This should only occur is an ability is changing it's input buffer enabled status
					UE_LOG(LogThresholdGeneral, Warning, TEXT("GameplayAbility %s is in the input "
                           "buffer of %s but does not have input buffering enabled"),
                           *GameplayAbility->GetName(), *GetNameSafe(this))
					RemoveFrontInput();
					continue;
				}

				if (GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get()))
				{
					// Ready the input and activate the ability!
					MostRecentInputData = Input->Data;
					TryActivateAbility(Spec.Handle);
					RemoveFrontInput();
				}
			}

			// If we can't activate the first ability in the buffer, then we should stop
			bFoundBlockedInput = true;
			break;
		}
	}
}






// Local gameplay cue functions

void UTHAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag,
		EGameplayCueEvent::Executed, GameplayCueParameters);
}

void UTHAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag,
		EGameplayCueEvent::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag,
		EGameplayCueEvent::WhileActive, GameplayCueParameters);
}

void UTHAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag,
		EGameplayCueEvent::Removed, GameplayCueParameters);
}




// Input buffer helpers

void UTHAbilitySystemComponent::BufferInput(FBufferedInput&& Input)
{
	// Used to track number of buffered inputs since TQueue does not hold a size
	CurrentInputBufferSize++;

	while (CurrentInputBufferSize > MaxInputBufferSize)
	{
		// Remove the front input if the buffer is full
		CurrentInputBufferSize--;
		InputBuffer.Pop();
	}
					
	// Push it back to our input buffer
	InputBuffer.Enqueue(Input);
}

void UTHAbilitySystemComponent::RemoveFrontInput()
{
	if (InputBuffer.IsEmpty())
	{
		return;
	}

	CurrentInputBufferSize--;
	InputBuffer.Pop();
}

