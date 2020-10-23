// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_GameplayEvent.generated.h"



/**
 * This class fires off a generic GAS event when the notify is triggered
 * It is particularly useful when used in conjunction with the PlayMontageAndWaitForEvent ability task
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories,
    meta=(ShowWorldContextPin, DisplayName="GameplayEvent"))
class THRESHOLDGAME_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameplayTag")
	FGameplayTag EventTag;
};
