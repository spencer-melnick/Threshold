// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Threshold/Abilities/AbilityInputTypes.h"
#include "THGameplayAbility.generated.h"



UCLASS()
class UTHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTHGameplayAbility();
	
	

	UPROPERTY()
	EAbilityInputType DefaultInputBinding = EAbilityInputType::None;
	
	// Tells our gameplay ability that it will be fed the directional input
	// from the character input vector on any activation calls via event data
	UPROPERTY()
	bool bRequiresDirectionInput = false;
};
