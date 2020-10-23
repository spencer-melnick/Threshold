// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/Tasks/AT_ApplySlowdownCurve.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "ThresholdGame.h"


// UAT_ApplySlowdownCurve

UAT_ApplySlowdownCurve::UAT_ApplySlowdownCurve()
{
	// Enable ticking
	bTickingTask = true;
}

UAT_ApplySlowdownCurve* UAT_ApplySlowdownCurve::ApplySlowdownCurve(UGameplayAbility* OwningAbility, FName TaskInstanceName, UCurveFloat* SlowdownCurve, float Duration)
{
	UAT_ApplySlowdownCurve* AbilityTask = NewAbilityTask<UAT_ApplySlowdownCurve>(OwningAbility, TaskInstanceName);
	AbilityTask->SlowdownCurve = SlowdownCurve;
	AbilityTask->Duration = Duration;

	return AbilityTask;
}





// Ability task overrides

void UAT_ApplySlowdownCurve::Activate()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogThresholdGame, Warning, TEXT("UAT_ApplySlowdownCurve called by %s with no AbilitySystemComponent in instance %s"),
            *GetNameSafe(Ability), *InstanceName.ToString())
		return;
	}

	if (!SlowdownCurve)
	{
		UE_LOG(LogThresholdGame, Warning, TEXT("UAT_ApplySlowdownCurve called by %s with no SlowdownCurve in instance %s"),
            *GetNameSafe(Ability), *InstanceName.ToString())
		return;
	}

	bIsFinished = false;
}

void UAT_ApplySlowdownCurve::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	// Accumulate time
	ElapsedTime += DeltaTime;

	if (ElapsedTime > Duration)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			// Broadcast delegates
			OnEnd.Broadcast();
		}
		
		// If the task has exceeded its duration, exit
		bIsFinished = true;
		EndTask();
		return;
	}

	if (!SlowdownCurve)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		// Set the montage playback rate
		const float NewPlayRate = SlowdownCurve->GetFloatValue(ElapsedTime);
		AbilitySystemComponent->CurrentMontageSetPlayRate(NewPlayRate);
	}
}

void UAT_ApplySlowdownCurve::PreDestroyFromReplication()
{
	bIsFinished = true;
	EndTask();
}

void UAT_ApplySlowdownCurve::OnDestroy(bool bInOwnerFinished)
{
	if (AbilitySystemComponent)
	{
		// Restore the original playback rate
		AbilitySystemComponent->CurrentMontageSetPlayRate(1.f);
	}

	Super::OnDestroy(bInOwnerFinished);
}
