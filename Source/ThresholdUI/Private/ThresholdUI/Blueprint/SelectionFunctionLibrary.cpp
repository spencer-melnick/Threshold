// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Blueprint/SelectionFunctionLibrary.h"



ESelectionDirection USelectionFunctionLibrary::FindOppositeDirection(const ESelectionDirection InSelectionDirection)
{
	switch (InSelectionDirection)
	{
		case ESelectionDirection::Up:
			return ESelectionDirection::Down;
		case ESelectionDirection::Left:
			return ESelectionDirection::Right;
		case ESelectionDirection::Down:
			return ESelectionDirection::Up;
		case ESelectionDirection::Right:
			return ESelectionDirection::Left;
		default:
			return ESelectionDirection::None;
	}
}

FIntPoint USelectionFunctionLibrary::GetUnitCoordinateFromDirection(const ESelectionDirection Direction)
{
	switch (Direction)
	{
		case ESelectionDirection::Up:
			return FIntPoint(0, -1);
		case ESelectionDirection::Left:
			return FIntPoint(-1, 0);
		case ESelectionDirection::Down:
			return FIntPoint(0, 1);
		case ESelectionDirection::Right:
			return FIntPoint(1, 0);
		default:
			return FIntPoint::ZeroValue;
	}
}

