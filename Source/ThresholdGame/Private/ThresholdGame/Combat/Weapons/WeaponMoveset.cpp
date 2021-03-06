// Copyright (c) 2020 Spencer Melnick


#include "ThresholdGame/Combat/Weapons/WeaponMoveset.h"

const FWeaponMove* UWeaponMoveset::GetWeaponMove(int32 WeaponMoveIndex) const
{
	if (WeaponMoveIndex < 0 || WeaponMoveIndex >= WeaponMoves.Num())
	{
		return nullptr;
	}

	return &WeaponMoves[WeaponMoveIndex];
}

int32 UWeaponMoveset::GetNextWeaponMoveIndex(int32 CurrentMoveIndex, EWeaponMoveType NextMoveType) const
{
	if (CurrentMoveIndex < 0)
	{
		// We are looking for the first weapon move
		switch (NextMoveType)
		{
			case EWeaponMoveType::Primary:
				return StartingPrimaryMoveIndex;
			case EWeaponMoveType::Secondary:
				return StartingSecondaryMoveIndex;
		}
	}

	const FWeaponMove* CurrentWeaponMove = GetWeaponMove(CurrentMoveIndex);

	if (!CurrentWeaponMove)
	{
		return -1;
	}

	switch (NextMoveType)
	{
		case EWeaponMoveType::Primary:
			return CurrentWeaponMove->PrimaryFollowupIndex;
		case EWeaponMoveType::Secondary:
			return CurrentWeaponMove->SecondaryFollowupIndex;
	}

	// How did you get here?
	return -1;
}

