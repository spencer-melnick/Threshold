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

		if (GameplayAbility->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
		{
			// If the ability is instanced, we can do the rest of our checks on the primary instance
			GameplayAbility = Cast<UTHGameplayAbility>(Spec.GetPrimaryInstance());
		}
		// If the ability is valid and instanced per actor, but doesn't have a valid instance, something went very wrong
		check(GameplayAbility);
		
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
			}
		}
		else
		{
			// Generate some input data for buffered abilities
			TSharedPtr<FBufferedAbilityInputData> InputData =
                GameplayAbility->GenerateInputData(Spec.Handle, AbilityActorInfo.Get());

			const bool bCanRetriggerAbility = GameplayAbility->CanBeRetriggered(Spec.Handle, AbilityActorInfo.Get());
			const bool bCanActivateAbility = GameplayAbility->CanActivateAbility(Spec.Handle, AbilityActorInfo.Get());

			if (bCanActivateAbility && (!Spec.IsActive() || bCanRetriggerAbility))
			{
				// Set the input data and activate the ability
				MostRecentInputData = InputData;
				TryActivateAbility(Spec.Handle);
			}
			else if (bEnableInputBuffering)
			{
				// Buffer the input
				// TODO: Store some unique identifier in the input buffer so we can find the correct spec later
				BufferInput({InputID, InputData, GetWorld()->GetRealTimeSeconds()});
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

		// Because of this, we can only buffer input for one ability per InputID
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
		
		if (GameplayAbility->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
        {
        	// If the ability is instanced, we can do the rest of our checks on the primary instance
        	GameplayAbility = Cast<UTHGameplayAbility>(FoundSpec->GetPrimaryInstance());
        }
        // If the ability is valid and instanced per actor, but doesn't have a valid instance, something went very wrong
        check(GameplayAbility);

		const bool bCanRetriggerAbility = GameplayAbility->CanBeRetriggered(FoundSpec->Handle, AbilityActorInfo.Get());
		const bool bCanActivateAbility = GameplayAbility->CanActivateAbility(FoundSpec->Handle, AbilityActorInfo.Get());
		
		if (bCanActivateAbility && (!FoundSpec->IsActive() || bCanRetriggerAbility))
		{
			// Ready the input and activate the ability
			MostRecentInputData = Input->Data;
			TryActivateAbility(FoundSpec->Handle);
			RemoveFrontInput();
		}
		else
		{
			// If we can't activate the ability then it is blocking the buffer and we should stop
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


