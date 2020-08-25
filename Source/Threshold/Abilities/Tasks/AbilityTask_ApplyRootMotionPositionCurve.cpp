// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "AbilityTask_ApplyRootMotionPositionCurve.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "Threshold/Abilities/Motion/THMotionSources.h"

UAbilityTask_ApplyRootMotionPositionCurve* UAbilityTask_ApplyRootMotionPositionCurve::ApplyRootMotionPositionCurve(
    UGameplayAbility* OwningAbility,
    FName TaskInstanceName,
    FVector Direction,
    float Scale,
    float Duration,
    UCurveFloat* PositionOverTime)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

    UAbilityTask_ApplyRootMotionPositionCurve* Task =
        NewAbilityTask<UAbilityTask_ApplyRootMotionPositionCurve>(OwningAbility, TaskInstanceName);

    Task->ForceName = TaskInstanceName;
    Task->Direction = Direction;
    Task->Scale = Scale;
    Task->Duration = Duration;
    Task->PositionOverTime = PositionOverTime;
    Task->SharedInitAndApply();

    return Task;
}

void UAbilityTask_ApplyRootMotionPositionCurve::SharedInitAndApply()
{
    if (AbilitySystemComponent->AbilityActorInfo->MovementComponent.IsValid())
    {
        MovementComponent = Cast<UCharacterMovementComponent>(AbilitySystemComponent->AbilityActorInfo->MovementComponent.Get());

        if (MovementComponent)
        {
            ForceName = ForceName.IsNone() ? FName(TEXT("AbilityTaskApplyRootMotionPositionCurve")) : ForceName;
            FRootMotionSource_PositionCurve* RootMotionSource = new FRootMotionSource_PositionCurve();
            RootMotionSource->InstanceName = ForceName;
            RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
            RootMotionSource->Priority = 6;
            RootMotionSource->Direction = Direction;
            RootMotionSource->Scale = Scale;
            RootMotionSource->Duration = Duration;
            RootMotionSource->PositionOverTime = PositionOverTime;
            RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSource);

            if (Ability)
            {
                Ability->SetMovementSyncPoint(ForceName);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_ApplyRootMotionPositionCurve called in Ability %s with null MovementComponent; Task Instance name %s"),
            *GetNameSafe(Ability), *InstanceName.ToString())
    }
}




// The following function implementation is adapted from original Unreal Engine Code
// The original code as part of the Unreal Engine is
// Copyright Epic Games, Inc. All Rights Reserved.

void UAbilityTask_ApplyRootMotionPositionCurve::TickTask(float DeltaTime)
{
    if (bIsFinished)
    {
        return;
    }

    Super::TickTask(DeltaTime);

    AActor* MyActor = GetAvatarActor();
    if (MyActor)
    {
        const bool bTimedOut = HasTimedOut();
        const bool bIsInfiniteDuration = Duration < 0.f;

        if (!bIsInfiniteDuration && bTimedOut)
        {
            // Task has finished
            bIsFinished = true;

            if (!bIsSimulating)
            {
                MyActor->ForceNetUpdate();
                if (ShouldBroadcastAbilityTaskDelegates())
                {
                    OnFinish.Broadcast();
                }
                EndTask();
            }
        }
    }
    else
    {
        bIsFinished = true;
        EndTask();
    }
}

void UAbilityTask_ApplyRootMotionPositionCurve::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAbilityTask_ApplyRootMotionPositionCurve, Direction);
    DOREPLIFETIME(UAbilityTask_ApplyRootMotionPositionCurve, Scale);
    DOREPLIFETIME(UAbilityTask_ApplyRootMotionPositionCurve, Duration);
    DOREPLIFETIME(UAbilityTask_ApplyRootMotionPositionCurve, PositionOverTime);
}

void UAbilityTask_ApplyRootMotionPositionCurve::PreDestroyFromReplication()
{
    bIsFinished = true;
    EndTask();
}

void UAbilityTask_ApplyRootMotionPositionCurve::OnDestroy(bool bInOwnerFinished)
{
    if (MovementComponent)
    {
        MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
    }

    Super::OnDestroy(bInOwnerFinished);
}
