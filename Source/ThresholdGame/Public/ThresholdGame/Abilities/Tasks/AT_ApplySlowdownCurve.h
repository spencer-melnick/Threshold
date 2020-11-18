// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_ApplySlowdownCurve.generated.h"



// Forward declarations

class UAbilitySystemComponent;



// Delegate declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FApplySlowdownCurveDelegate);

/**
 * Applies a play rate change to the active montage from a float curve
 */
UCLASS()
class THRESHOLDGAME_API UAT_ApplySlowdownCurve : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_ApplySlowdownCurve();

	static UAT_ApplySlowdownCurve* ApplySlowdownCurve(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* Montage,
		UCurveFloat* SlowdownCurve,
		float Duration,
		float StartTime = -1.f);

	
	// Ability task overrides

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
	virtual void TickTask(float DeltaTime) override;
	virtual void PreDestroyFromReplication() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	// Delegates

	UPROPERTY(BlueprintAssignable)
	FApplySlowdownCurveDelegate OnEnd;


protected:

	void SharedInitAndApply();

	UPROPERTY(Replicated)
	UAnimMontage* Montage;
	
	UPROPERTY(Replicated)
	UCurveFloat* SlowdownCurve;

	UPROPERTY(Replicated)
	float Duration;

	UPROPERTY(Replicated)
	float StartTime;


private:
	// Private execution variables
	
	float ElapsedTime = 0.f;
	bool bIsFinished = true;

	UPROPERTY()
	UAnimInstance* AnimInstance;
};
