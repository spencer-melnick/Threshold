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
class UAT_ApplySlowdownCurve : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_ApplySlowdownCurve();

	static UAT_ApplySlowdownCurve* ApplySlowdownCurve(UGameplayAbility* OwningAbility, FName TaskInstanceName, UCurveFloat* SlowdownCurve, float Duration);

	
	// Ability task overrides

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void PreDestroyFromReplication() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	// Delegates

	UPROPERTY(BlueprintAssignable)
	FApplySlowdownCurveDelegate OnEnd;


protected:
	UPROPERTY()
	UCurveFloat* SlowdownCurve;
	float Duration;


private:
	// Private execution variables
	
	float ElapsedTime = 0.f;
	bool bIsFinished = true;
};
