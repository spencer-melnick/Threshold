// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Threshold/Abilities/THGameplayAbility.h"
#include "WeaponAttack.generated.h"



// Forward declarations

class UAnimMontage;



/**
 * A simple weapon attack ability - not finished yet
 */
UCLASS()
class UWeaponAttack : public UTHGameplayAbility
{
	GENERATED_BODY()

public:
	UWeaponAttack();



	// Engine overrides

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		OUT FGameplayTagContainer* OptionalRelevantTags) const override;



	// Editor properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	UAnimMontage* AttackMontage = nullptr;



protected:

	// Task callbacks
	
	UFUNCTION()
	void OnAnimationFinished();

	UFUNCTION()
	void OnInputPressed(float ElapsedTime);
};
