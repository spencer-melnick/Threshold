// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "THCharacterAnim.generated.h"

/**
 * 
 */
UCLASS()
class THRESHOLD_API UTHCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void Dodge();

	UFUNCTION(BlueprintCallable, Category="Character")
	void EndDodge();

	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void CheckIsDodging(bool& bIsDodging);
	
};
