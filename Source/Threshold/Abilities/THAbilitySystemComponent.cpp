// Copyright © 2020 Spencer Melnick

#include "THAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "THGameplayAbility.h"
#include "TargetDataTypes.h"



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
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
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
				else
				{
					UTHGameplayAbility* GameplayAbility = Cast<UTHGameplayAbility>(Spec.Ability);
					if (GameplayAbility)
					{
						if (!GameplayAbility->bRequiresDirectionInput)
						{
							// Activate our ability normally
							TryActivateAbility(Spec.Handle);
						}
						else
						{
							// Try to get our character
							ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor);

							if (OwningCharacter)
							{
								// Grab the movement direction and build event data
								const FVector InputDirection = OwningCharacter->GetLastMovementInputVector();
								FAbilityDirectionalData* DirectionalData = new FAbilityDirectionalData();
								DirectionalData->Direction = InputDirection.GetSafeNormal();
								FGameplayEventData EventData;
								EventData.TargetData.Add(DirectionalData);

								// Trigger the ability with our new event data
								FScopedPredictionWindow NewPredictionWindow(this, true);
								TriggerAbilityFromGameplayEvent(Spec.Handle, nullptr, FGameplayTag::EmptyTag, &EventData, *this);
							}
						}
					}
				}
			}
		}
	}
}

