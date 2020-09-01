// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_Tag.generated.h"



// Forward declarations
class UTHAbilitySystemComponent;

/**
 * This class is used to add local (non-replicated) gameplay tags to the animation owner's ability system component.
 * This is used whenever effects are driven by an animation montage, since the montage will be replicated to all
 * clients already and by driving effects locally we limit the amount of RPCs being called, while still maintaining
 * the flexibility of gameplay tags.
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories,
	meta=(ShowWorldContextPin, DisplayName="GameplayTag"))
class UAnimNotifyState_Tag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameplayTag")
	FGameplayTagContainer AppliedTags;

protected:
	static UAbilitySystemComponent* GetAbilitySystemComponent(USkeletalMeshComponent* MeshComponent);
};
