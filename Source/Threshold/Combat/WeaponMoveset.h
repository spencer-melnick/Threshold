// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
