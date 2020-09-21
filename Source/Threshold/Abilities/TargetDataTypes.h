// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "TargetDataTypes.generated.h"



/**
 * Struct used for storing directional input data and transmitting it during a gameplay ability call
 */
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



/**
 * Struct used for weapon hit event data
 */
USTRUCT()
struct FWeaponHitTargetData : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FWeaponHitTargetData() :
		Super(), HitVelocity(ForceInitToZero)
	{}

	FWeaponHitTargetData(FHitResult InHitResult, FVector InHitVelocity) :
		Super(MoveTemp(InHitResult)), HitVelocity(InHitVelocity)
	{}

	UPROPERTY()
	FVector HitVelocity;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FWeaponHitTargetData::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FWeaponHitTargetData");
	}

	// Ignore this warning - I know what I'm doing!
	// (FGameplayAbilityTargetData calls the NetSerialize() function of the struct, if this wasn't named NetSerialize
	// the extra data wouldn't be serialized, it NEEDS to hide the base struct's function. We call Super::NetSerialize()
	// to make sure that the base struct data is serialized though)
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};




/**
 * Target data that just holds a single integer value
 * (Is this entirely necessary? It seems like I'm just using target data for my own generic data payloads at this point)
 */
USTRUCT()
struct FIntegralTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 IntegralData = 0;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FIntegralTargetData::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FIntegralTargetData");
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};



/**
 * Target data that holds a single target - may not be an actor, but any UObject type
 */
USTRUCT()
struct FSingleObjectTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UObject> Object;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSingleObjectTargetData::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FSingleObjectTargetData");
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
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

template<>
struct TStructOpsTypeTraits<FWeaponHitTargetData> : public TStructOpsTypeTraitsBase2<FWeaponHitTargetData>
{
	enum
	{
		WithNetSerializer = true
	};
};

template<>
struct TStructOpsTypeTraits<FIntegralTargetData> : public TStructOpsTypeTraitsBase2<FIntegralTargetData>
{
	enum
	{
		WithNetSerializer = true
    };
};

template<>
struct TStructOpsTypeTraits<FSingleObjectTargetData> : public TStructOpsTypeTraitsBase2<FSingleObjectTargetData>
{
	enum
	{
		WithNetSerializer = true
	};
};
