// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Threshold/Threshold.h"
#include "AbilityFunctionLibrary.generated.h"

/**
 * Simple function library for reusable ability functions, such as spawning an event/cue outside of a gameplay
 * effect
 */
UCLASS()
class UAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ability")
	static FGameplayEventData CreateGameplayEvent(AActor* Instigator, AActor* Target, FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category="Ability")
	static FGameplayCueParameters CreateGameplayCue(AActor* Instigator, AActor* EffectCauser, AActor* Target);

	template<typename T>
	static const T* ConvertTargetData(const FGameplayAbilityTargetDataHandle& Data)
	{
		if (!Data.IsValid(0) || Data.Num() != 1)
		{
			// Check to see that we actually got the right amount of valid data
			UE_LOG(LogThresholdGeneral, Error, TEXT("Received wrong amount of target data from client; expected 1 got %d"), Data.Num());
			return nullptr;
		}

		const FGameplayAbilityTargetData* TargetData = Data.Get(0);

		if (TargetData->GetScriptStruct() != T::StaticStruct())
		{
			// Check if the data type is actually correct before attempting to cast
			UE_LOG(LogThresholdGeneral, Error, TEXT("Received incorrect target data type from client, expected %s, got %s"),
                *T::StaticStruct()->GetStructCPPName(), *TargetData->GetScriptStruct()->GetStructCPPName())
			return nullptr;
		}
	
		return static_cast<const T*>(TargetData);
	}
};
