// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdUI/Types/SelectionTypes.h"
#include "SelectionFunctionLibrary.generated.h"



UCLASS()
class USelectionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static ESelectionDirection FindOppositeDirection(const ESelectionDirection InSelectionDirection);

	UFUNCTION(BlueprintCallable)
	static FIntPoint GetUnitCoordinateFromDirection(const ESelectionDirection Direction);
};
