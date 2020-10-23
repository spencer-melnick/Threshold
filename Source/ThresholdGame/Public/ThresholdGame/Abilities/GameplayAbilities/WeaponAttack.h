// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdGame/Abilities/THGameplayAbility.h"
#include "WeaponAttack.generated.h"



// Forward declarations

class UAnimMontage;
class UWeaponMoveset;
class UGameplayEffect;



/**
 * A simple weapon attack ability - not finished yet
 */
UCLASS()
class THRESHOLDGAME_API UWeaponAttack : public UTHGameplayAbility
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



	// Input buffering overrides
	
	virtual bool GetInputBufferingEnabled() const override
	{
		return true;
	}



	// Editor properties
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	FGameplayTagContainer HitEventTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	FGameplayTag BaseDamageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	float BaseDamageAmount = 10.f;



protected:
	// Task callbacks
	
	UFUNCTION()
	void OnAnimationFinished(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
