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
		UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);

		if (Spec.InputID != InputID || !GameplayAbility)
		{
			continue;
		}
		
		Spec.InputPressed = true;

		// We can replicate an input pressed event immediately if the ability doesn't have input buffering or it's currently accepting secondary input
		const bool bReplicateInputImmediately = !GameplayAbility->GetInputBufferingEnabled() || GameplayAbility->GetCanAcceptInputPressed(Spec.Handle, AbilityActorInfo.Get());

		if (!GameplayAbility->GetInputBufferingEnabled())
		{
			// Handle unbuffered abilities
			if (Spec.IsActive())
			{
				DispatchInputEvents(Spec);
			}
			else
			{
				TryActivateAbility(Spec.Handle);
			}
		}
		else
		{
			// Generate some input data for buffered abilities
			TSharedPtr<FBufferedAbilityInputData> InputData =
                GameplayAbility->GenerateInputData(Spec.Handle, AbilityActorInfo.Get());

			if (Spec.IsActive())
			{
				// If the ability is already active
				if (GameplayAbility->GetCanAcceptInputPressed(Spec.Handle, AbilityActorInfo.Get()))
				{
					// but we can accept a new input currently, dispatch it immediately
					DispatchInputEvents(Spec);
				}
				else
				{
					// otherwise buffer the activation
					BufferInput({InputID, InputData, GetWorld()->GetRealTimeSeconds()});
				}
			}
			else
			{
				// If the ability isn't active
				if (GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get()))
				{
					// and we can activate the ability make the data available and activate the ability normally
					MostRecentInputData = InputData;
					TryActivateAbility(Spec.Handle);
				}
				else if (bEnableInputBuffering)
				{
					// If the ability isn't active, but we can't activate for some other reason buffer the activation
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

	while (!InputBuffer.IsEmpty())
	{			
		FBufferedInput* Input = InputBuffer.Peek();
		const float InputID = Input->InputID;

		if (GetWorld()->GetRealTimeSeconds() - Input->InputTime > InputBufferingTime)
		{
			// Remove expired inputs and check the next input
			RemoveFrontInput();
			continue;
		}

		FGameplayAbilitySpec* FoundSpec;

		{
			ABILITYLIST_SCOPE_LOCK();
			FoundSpec = ActivatableAbilities.Items.FindByPredicate([InputID](const FGameplayAbilitySpec& Spec)
			{
				return Spec.InputID == InputID;
			});
		}

		if (!FoundSpec)
		{
			// There is no ability to trigger this input
			RemoveFrontInput();
			continue;
		}

		UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(FoundSpec->Ability);

		if (!GameplayAbility || !GameplayAbility->GetInputBufferingEnabled())
		{
			// If we have an ability that shouldn't be buffered, warn and remove it from the input buffer
			// This should only occur if an ability is changing it's input buffer enabled status
			UE_LOG(LogThresholdGeneral, Warning, TEXT("GameplayAbility %s is in the input "
                   "buffer of %s but is not a THGameplayAbility or does not have input buffering enabled"),
                   *GameplayAbility->GetName(), *GetNameSafe(this))
			RemoveFrontInput();
			continue;
		}
		
		FoundSpec->InputPressed = true;
		if (!FoundSpec->IsActive())
		{
			if (GameplayAbility->CanActivateAbility(FoundSpec->Handle, AbilityActorInfo.Get()))
			{
				// If the ability isn't active and we can activate it, ready the input and activate the ability
				MostRecentInputData = Input->Data;
				TryActivateAbility(FoundSpec->Handle);
				RemoveFrontInput();
			}
			else
			{
				// If the ability isn't active currently, but can't be activated, then it is blocking the buffer and we should stop
				break;
			}
		}
		else if (GameplayAbility->GetCanAcceptInputPressed(FoundSpec->Handle, AbilityActorInfo.Get()))
		{
			// If the ability is currently active, but it can accept a secondary input, dispatch the event
			DispatchInputEvents(*FoundSpec);
			RemoveFrontInput();
		}
		else
		{
			// If the ability is active, but can't accept a secondary input, then it is blocking and we should stop
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

void UTHAbilitySystemComponent::DispatchInputEvents(FGameplayAbilitySpec& Spec)
{
	if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
	{
		ServerSetInputPressed(Spec.Handle);
	}

	AbilitySpecInputPressed(Spec);

	// TODO: What is this prediction key? We might need to generate a new one, since this isn't necessarily running on activation
	// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
}


