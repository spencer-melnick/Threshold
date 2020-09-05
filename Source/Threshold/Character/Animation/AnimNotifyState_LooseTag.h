// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_LooseTag.generated.h"



// Forward declarations
class UTHAbilitySystemComponent;

/**
 * This class is used to add local (non-replicated) gameplay tags to the animation owner's ability system component.
 * This is used whenever effects are driven by an animation montage, since the montage will be replicated to all
 * clients already and by driving effects locally we limit the amount of RPCs being called, while still maintaining
 * the flexibility of gameplay tags.
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories,
	meta=(ShowWorldContextPin, DisplayName="LooseTag"))
class UAnimNotifyState_LooseTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameplayTag")
	FGameplayTagContainer AppliedTags;

	// If this is set, tags will be extended on the server - this is useful if tags are used for combo windows, because
	// extending them on the server will allow for greater leniency when clients request a combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bExtendTagOnServer = false;

protected:
	static UAbilitySystemComponent* GetAbilitySystemComponent(USkeletalMeshComponent* MeshComponent);
};
