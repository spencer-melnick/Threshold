// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Abilities/Motion/THMotionSources.h"
#include "Curves/CurveFloat.h"

//
// FRootMotionSource_PositionCurve
//

FRootMotionSource_PositionCurve::FRootMotionSource_PositionCurve()
    : Direction(FVector::ForwardVector)
{
    // This setting was grabbed from the constant force code,
    // and should help with consistent ending velocity
    Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);

    // Don't affect Z velocity
    Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
}

FRootMotionSource* FRootMotionSource_PositionCurve::Clone() const
{
    return new FRootMotionSource_PositionCurve(*this);
}

bool FRootMotionSource_PositionCurve::Matches(const FRootMotionSource* Other) const
{
    if (!FRootMotionSource::Matches(Other))
    {
        return false;
    }

    const FRootMotionSource_PositionCurve* OtherCast = static_cast<const FRootMotionSource_PositionCurve*>(Other);

    return PositionOverTime == OtherCast->PositionOverTime &&
        FMath::IsNearlyEqual(Scale, OtherCast->Scale) &&
        FVector::PointsAreNear(Direction, OtherCast->Direction, 0.1f);
}

bool FRootMotionSource_PositionCurve::MatchesAndHasSameState(const FRootMotionSource* Other) const
{
    return FRootMotionSource::MatchesAndHasSameState(Other);
}

bool FRootMotionSource_PositionCurve::UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup)
{
    return FRootMotionSource::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup);
}

void FRootMotionSource_PositionCurve::PrepareRootMotion(
    float SimulationTime,
    float MovementTickTime,
    const ACharacter& Character,
    const UCharacterMovementComponent& MoveComponent)
{
    RootMotionParams.Clear();

    // Check for valid position curve and duration
    checkf(PositionOverTime && Duration > SMALL_NUMBER,
        TEXT("FRootMotionSource_PositionCurve has invalid curve or duration"));

    const float NewTimeFractional = FMath::Clamp((GetTime() + SimulationTime) / Duration, 0.f, 1.f);
    const float PreviousTimeFractional = FMath::Clamp((GetTime()) / Duration, 0.f, 1.f);

    // Only apply force if the time changes are significant
    if (NewTimeFractional - PreviousTimeFractional > SMALL_NUMBER && MovementTickTime > SMALL_NUMBER)
    {
        const FVector PreviousOffset = Direction * Scale * PositionOverTime->GetFloatValue(PreviousTimeFractional);
        const FVector NewOffset = Direction * Scale * PositionOverTime->GetFloatValue(NewTimeFractional);

        const FVector Force = (NewOffset - PreviousOffset) / MovementTickTime;
        const FTransform NewTransform(Force);
        RootMotionParams.Set(NewTransform);
    }

    // Accumulate time
    SetTime(GetTime() + SimulationTime);
}

bool FRootMotionSource_PositionCurve::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    if (!FRootMotionSource::NetSerialize(Ar, Map, bOutSuccess))
    {
        return false;
    }

    Ar << Direction;
    Ar << PositionOverTime;
    Ar << Scale;

    bOutSuccess = true;
    return true;
}

UScriptStruct* FRootMotionSource_PositionCurve::GetScriptStruct() const
{
    return FRootMotionSource_PositionCurve::StaticStruct();
}

FString FRootMotionSource_PositionCurve::ToSimpleString() const
{
    return FString::Printf(TEXT("[ID:%ud]FRootMotionSource_PositionCurve %s"), LocalID,
        *InstanceName.GetPlainNameString());
}

void FRootMotionSource_PositionCurve::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(PositionOverTime);

    FRootMotionSource::AddReferencedObjects(Collector);
}
