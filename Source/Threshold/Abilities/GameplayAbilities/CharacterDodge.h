﻿// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Threshold/Abilities/THGameplayAbility.h"
#include "CharacterDodge.generated.h"


UCLASS(Abstract)
class UCharacterDodge : public UTHGameplayAbility
{
	GENERATED_BODY()

public:
	UCharacterDodge();

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
	
	UPROPERTY(EditAnywhere)
	class UCurveFloat* PositionCurve = nullptr;

	UPROPERTY(EditAnywhere)
	float DodgeDistance = 100.f;

	UPROPERTY(EditAnywhere)
	float DodgeDuration = 0.3f;

	// This tag is applied to the ability system component locally by the task - that means that it is not replicated,
	// but it should be the same across all machines. This is used for changing visual properties when a dodge
	// begins, not for any gameplay code
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag LocalDodgeTag;

protected:
	void ApplyDodgeMotionTask(const FVector Direction);

	UFUNCTION()
	void OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void OnDodgeFinished();
};
