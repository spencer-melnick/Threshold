// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ThresholdGame/Combat/DamageTypes.h"
#include "WeaponMoveset.generated.h"

/**
 * 
 */

UENUM()
enum class EWeaponMoveType : int8
{
	Primary,
	Secondary
};

UENUM()
enum class EWeaponMoveBlockLevel : int8
{
	AlwaysBlockable,
	NeverBlockable
};

USTRUCT(BlueprintType)
struct THRESHOLDGAME_API FWeaponMove
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Animation = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 PrimaryFollowupIndex = -1;

	UPROPERTY(EditDefaultsOnly)
	int32 SecondaryFollowupIndex = -1;

	// How much to scale the base damage of this attack by.
	// Allows for certain moves to do more damage
	UPROPERTY(EditDefaultsOnly)
	float DamageScale = 1.f;

	// The requirements for the character to be able to block this
	// attack. Some attacks are not blockable
	UPROPERTY(EditDefaultsOnly)
	EWeaponMoveBlockLevel BlockLevel = EWeaponMoveBlockLevel::AlwaysBlockable;
};



UCLASS()
class THRESHOLDGAME_API UWeaponMoveset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FWeaponMove* GetWeaponMove(int32 WeaponMoveIndex) const;
	
	// Returns the next weapon move given the current weapon move or -1 if there is none.
	// If CurrentWeaponMoveIndex is -1, then it will return the starting weapon move index, if any.
	int32 GetNextWeaponMoveIndex(int32 CurrentMoveIndex, EWeaponMoveType NextMoveType) const;

	bool IsValidMove(int32 WeaponMoveIndex) const
	{
		return WeaponMoveIndex >= 0 && WeaponMoveIndex < WeaponMoves.Num();
	}

	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 StartingPrimaryMoveIndex = -1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 StartingSecondaryMoveIndex = -1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FWeaponMove> WeaponMoves;
};
