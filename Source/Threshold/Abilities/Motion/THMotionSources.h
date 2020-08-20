// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "THMotionSources.generated.h"


/**
 * This root motion source takes a normalized direction vector,
 * a normalized position over time curve and a scale. It will
 * apply a force equal to the derivative of the position over
 * time curve with the appropriate direction and scale.
 *
 * This type of motion is useful for dodging, as instead of setting
 * the velocity as a curve it gives the designer a precise control
 * of how far the dodge should take the character and where exactly
 * along the path the character should be at a specific point in
 * time.
 */
USTRUCT()
struct FRootMotionSource_PositionCurve : public FRootMotionSource
{
    GENERATED_USTRUCT_BODY()

    FRootMotionSource_PositionCurve();

    virtual ~FRootMotionSource_PositionCurve() {}

    UPROPERTY()
    FVector Direction;

    UPROPERTY()
    UCurveFloat* PositionOverTime = nullptr;

    UPROPERTY()
    float Scale = 1.f;

    virtual FRootMotionSource* Clone() const override;

    virtual bool Matches(const FRootMotionSource* Other) const override;

    virtual bool MatchesAndHasSameState(const FRootMotionSource* Other) const override;

    virtual bool UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup) override;

    virtual void PrepareRootMotion(
        float SimulationTime,
        float MovementTickTime,
        const ACharacter& Character,
        const UCharacterMovementComponent& MoveComponent
        ) override;

    virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

    virtual UScriptStruct* GetScriptStruct() const override;

    virtual FString ToSimpleString() const override;

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};


// Type traits to allow for proper network serialization
template<>
struct TStructOpsTypeTraits<FRootMotionSource_PositionCurve> : public TStructOpsTypeTraitsBase2<FRootMotionSource_PositionCurve>
{
	enum
	{
		WithNetSerializer = true,
        WithCopy = true
    };
};
