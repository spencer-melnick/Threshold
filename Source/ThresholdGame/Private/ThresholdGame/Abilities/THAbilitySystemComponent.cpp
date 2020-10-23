// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/THAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "ThresholdGame/Abilities/THGameplayAbility.h"
#include "GameplayCueManager.h"
#include "ThresholdGame.h"


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

	FBufferedInput BufferedInput;
	BufferedInput.InputTime = GetWorld()->GetRealTimeSeconds();
	BufferedInput.InputID = InputID;

	bool bAbilityTriggered = false;
	bool bAnyAbilityBuffered = false;
	
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);

		if (Spec.InputID != InputID || !GameplayAbility)
		{
			continue;
		}

		Spec.InputPressed = true;
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
				bAbilityTriggered = true;
			}
		}
		else
		{
			// Generate some input data for buffered abilities
			TSharedPtr<FBufferedAbilityInputData> InputData =
                GameplayAbility->GenerateInputData(Spec.Handle, AbilityActorInfo.Get());

			if (!Spec.IsActive() && GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get()))
			{
				MostRecentInputData = InputData;
				TryActivateAbility(Spec.Handle);
				bAbilityTriggered = true;
				break;
			}
			else
			{
				// Add our data to the buffer list
				BufferedInput.Data.Add(Spec.Handle, InputData);
				bAnyAbilityBuffered = true;
			}
		}
	}

	if (!bAbilityTriggered && bAnyAbilityBuffered)
	{
		// If no ability was triggered, store all of our data in the input buffer
		BufferInput(MoveTemp(BufferedInput));
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
		const int32 InputID = Input->InputID;

		if (GetWorld()->GetRealTimeSeconds() - Input->InputTime > InputBufferingTime)
		{
			// Remove expired inputs and check the next input
			RemoveFrontInput();
			continue;
		}

		bool bAbilityTriggered = false;

		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.InputID != InputID)
			{
				continue;
			}
			
			UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);

			if (!GameplayAbility || !GameplayAbility->GetInputBufferingEnabled())
			{
				// If we have an ability that shouldn't be buffered, warn and remove it from the input buffer
				// This should only occur if an ability is changing it's input buffer enabled status
				UE_LOG(LogThresholdGame, Warning, TEXT("GameplayAbility %s is in the input "
                       "buffer of %s but is not a THGameplayAbility or does not have input buffering enabled"),
                       *GameplayAbility->GetName(), *GetNameSafe(this))
				continue;
			}
		
			if (!Spec.IsActive() && GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get()))
			{
				// Ready the input and activate the ability
				TSharedPtr<FBufferedAbilityInputData>* InputFound = Input->Data.Find(Spec.Handle);

				if (InputFound)
				{
					MostRecentInputData = *InputFound;
					TryActivateAbility(Spec.Handle);
					RemoveFrontInput();
					bAbilityTriggered = true;
					break;
				}
				else
				{
					UE_LOG(LogThresholdGame, Warning, TEXT("GameplayAbility %s is in the input "
				       "buffer of %s but does not have any generated input data"),
				       *GameplayAbility->GetName(), *GetNameSafe(this))
				}
			}
		}

		if (!bAbilityTriggered)
		{
			// If no abilities were triggered then the buffer is blocked and we should exit
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
	UpdateShouldTick();
}

void UTHAbilitySystemComponent::RemoveFrontInput()
{
	if (InputBuffer.IsEmpty())
	{
		return;
	}

	CurrentInputBufferSize--;
	InputBuffer.Pop();
	UpdateShouldTick();
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


