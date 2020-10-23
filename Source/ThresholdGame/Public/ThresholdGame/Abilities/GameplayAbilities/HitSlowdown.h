// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdGame/Abilities/THGameplayAbility.h"
#include "HitSlowdown.generated.h"


/**
 * Ability that should be triggered by a gameplay event. Applies a slowdown to the currently playing montage
 * of the owning character
 */
UCLASS()
class THRESHOLDGAME_API UHitSlowdown : public UTHGameplayAbility
{
	GENERATED_BODY()
public:
	UHitSlowdown();


	// Gameplay ability overrides

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;


	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Slowdown")
	UCurveFloat* SlowdownCurve = nullptr;

	// Slowdown duration in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Slowdown")
	float Duration = 0.3f;
	

protected:
	// Task delegates

	UFUNCTION()
	void OnSlowdownEnded();
};
