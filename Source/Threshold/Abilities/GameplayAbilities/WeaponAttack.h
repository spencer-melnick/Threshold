// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Threshold/Abilities/THGameplayAbility.h"
#include "WeaponAttack.generated.h"



// Forward declarations

class UAnimMontage;
class UWeaponMoveset;



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

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;



	// Input buffering overrides
	
	virtual bool GetInputBufferingEnabled() const override
	{
		return true;
	}

	virtual bool CanBeRetriggered(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo) const override;	



	// Editor properties
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	UAnimMontage* AttackMontage = nullptr;

	// Tag that must be applied locally for this ability to be able to trigger a combo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	FGameplayTag LocalComboTag;



protected:
	// Helper functions

	static UWeaponMoveset* GetMoveset(AActor* OwningActor);

	// Returns true if the playback was successful, or false if the ability should be ended
	bool PlayAttackAnimation(int32 WeaponMoveIndex);

	

	// Task callbacks
	
	UFUNCTION()
	void OnAnimationFinished();

	UFUNCTION()
	void OnServerReceiveData(const FGameplayAbilityTargetDataHandle& Data);

	bool bShouldCombo = false;
	int32 CurrentWeaponMoveIndex = -1;
};
