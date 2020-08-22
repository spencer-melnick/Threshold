// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Threshold/Abilities/AbilityInputTypes.h"
#include "THGameplayAbility.generated.h"



UCLASS(Abstract)
class UTHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTHGameplayAbility();
	
	

	UPROPERTY()
	EAbilityInputType DefaultInputBinding = EAbilityInputType::None;

protected:
	void SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData);
};
