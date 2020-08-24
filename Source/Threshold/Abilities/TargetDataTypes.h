// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "TargetDataTypes.generated.h"

USTRUCT()
struct FAbilityDirectionalData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	FAbilityDirectionalData() :
		Direction(ForceInitToZero)
	{};

	virtual ~FAbilityDirectionalData() {};

	UPROPERTY()
	FVector Direction;

	void operator=(const FAbilityDirectionalData& OtherData)
	{
		Direction = OtherData.Direction;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAbilityDirectionalData::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FAblityDirectionalData");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

// Type traits to allow for proper network serialization
template<>
struct TStructOpsTypeTraits<FAbilityDirectionalData> : public TStructOpsTypeTraitsBase2<FAbilityDirectionalData>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
    };
};
