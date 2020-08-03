// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Threshold/Combat/DamageTypes.h"
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
struct FWeaponMove
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditDefaultsOnly)
	bool bHasPrimaryFollowup = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasPrimaryFollowup"))
	uint16 PrimaryFollowupIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	bool bHasSecondaryFollowup = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasSecondaryFollowup"))
	uint16 SecondaryFollowupIndex = 0;

	// How much to scale the base damage of this attack by.
	// Allows for certain moves to do more damage
	UPROPERTY(EditDefaultsOnly)
	float DamageScale = 1.f;

	// The requirements for the character to be able to block this
	// attack. Some attacks are not blockable
	UPROPERTY(EditDefaultsOnly)
	EWeaponMoveBlockLevel BlockLevel = EWeaponMoveBlockLevel::AlwaysBlockable;

	TSubclassOf<class UDamageType> DamageType = USwordDamage::StaticClass();
};



UCLASS()
class THRESHOLD_API UWeaponMoveset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 StartingPrimaryMoveIndex = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 StartingSecondaryMoveIndex = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FWeaponMove> WeaponMoves;
};
