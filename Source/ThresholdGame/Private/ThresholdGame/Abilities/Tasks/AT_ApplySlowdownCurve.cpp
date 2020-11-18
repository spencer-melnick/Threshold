// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/Tasks/AT_ApplySlowdownCurve.h"
#include "ThresholdGame.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"



// UAT_ApplySlowdownCurve

UAT_ApplySlowdownCurve::UAT_ApplySlowdownCurve()
{
	// Enable ticking
	bTickingTask = true;
	bSimulatedTask = true;
}

UAT_ApplySlowdownCurve* UAT_ApplySlowdownCurve::ApplySlowdownCurve(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* Montage,
	UCurveFloat* SlowdownCurve,
	float Duration,
	float StartTime)
{
	UAT_ApplySlowdownCurve* AbilityTask = NewAbilityTask<UAT_ApplySlowdownCurve>(OwningAbility, TaskInstanceName);
	AbilityTask->Montage = Montage;
	AbilityTask->SlowdownCurve = SlowdownCurve;
	AbilityTask->Duration = Duration;
	AbilityTask->StartTime = StartTime;
	AbilityTask->SharedInitAndApply();

	return AbilityTask;
}





// Ability task overrides

void UAT_ApplySlowdownCurve::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UAT_ApplySlowdownCurve, Montage);
	DOREPLIFETIME(UAT_ApplySlowdownCurve, SlowdownCurve);
	DOREPLIFETIME(UAT_ApplySlowdownCurve, Duration);
	DOREPLIFETIME(UAT_ApplySlowdownCurve, StartTime);
}


void UAT_ApplySlowdownCurve::SharedInitAndApply()
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
	AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogThresholdGame, Warning, TEXT("%s called by %s: %s with no valid AnimInstance"),
            ANSI_TO_TCHAR(__FUNCTION__), *GetNameSafe(Ability), *InstanceName.ToString())
		return;
	}
	if (!Montage)
	{
		UE_LOG(LogThresholdGame, Warning, TEXT("%s called by %s: %s with no valid montage"),
            ANSI_TO_TCHAR(__FUNCTION__), *GetNameSafe(Ability), *InstanceName.ToString())
		return;
	}
	
	if (StartTime >= 0.f)
	{
		AnimInstance->Montage_SetPosition(Montage, StartTime);
	}
	bIsFinished = false;
}

void UAT_ApplySlowdownCurve::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	SharedInitAndApply();
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

	if (AnimInstance && Montage)
	{
		// Set the montage playback rate
		const float NewPlayRate = SlowdownCurve->GetFloatValue(ElapsedTime);
		AnimInstance->Montage_SetPlayRate(Montage, NewPlayRate);
		
	}
}

void UAT_ApplySlowdownCurve::PreDestroyFromReplication()
{
	bIsFinished = true;
	EndTask();
}

void UAT_ApplySlowdownCurve::OnDestroy(bool bInOwnerFinished)
{
	if (AnimInstance && Montage)
	{
		// Restore the original playback rate
		AnimInstance->Montage_SetPlayRate(Montage, 1.f);
	}

	Super::OnDestroy(bInOwnerFinished);
}
