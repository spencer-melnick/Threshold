// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakInterfacePtr.h"
#include "Threshold/Abilities/THGameplayAbility.h"
#include "InteractAbility.generated.h"


// Forward declarations

class IInteractiveObject;


/**
 * Ability that allows characters to interact with in world interactive objects. Right now there is no client side
 * prediction, and the actual detection of interactive objects is handled in the player controller. For the most
 * part this ability just connects the different classes together
 */
UCLASS()
class UInteractAbility : public UTHGameplayAbility
{
	GENERATED_BODY()

public:
	UInteractAbility();


	// Gameplay ability overrides

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;


protected:
	// Ability task callbacks
	
	UFUNCTION()
	void OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data);


	// Ability execution

	void TriggerInteraction(TWeakInterfacePtr<IInteractiveObject> Object);
};
