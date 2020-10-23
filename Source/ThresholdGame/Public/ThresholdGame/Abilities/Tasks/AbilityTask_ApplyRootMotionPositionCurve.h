// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "AbilityTask_ApplyRootMotionPositionCurve.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FApplyRootMotionPositionCurveDelegate);

UCLASS()
class THRESHOLDGAME_API UAbilityTask_ApplyRootMotionPositionCurve : public UAbilityTask_ApplyRootMotion_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FApplyRootMotionPositionCurveDelegate OnFinish;
    
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_ApplyRootMotionPositionCurve* ApplyRootMotionPositionCurve(
        UGameplayAbility* OwningAbility,
        FName TaskInstanceName,
        FVector Direction,
        float Scale,
        float Duration,
        class UCurveFloat* PositionOverTime,
        FGameplayTag LocallyAppliedTag
    );

    virtual void TickTask(float DeltaTime) override;
    virtual void PreDestroyFromReplication() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
	virtual void Activate() override;

protected:
    virtual void SharedInitAndApply() override;

    UPROPERTY(Replicated)
    FVector Direction;

    UPROPERTY(Replicated)
    float Scale;

    UPROPERTY(Replicated)
    float Duration;

    UPROPERTY(Replicated)
    class UCurveFloat* PositionOverTime = nullptr;

	// This tag is applied to the owning ability system component locally (not replicated) and removed once the root
	// motion ends
	UPROPERTY(Replicated)
	FGameplayTag LocallyAppliedTag;
};
