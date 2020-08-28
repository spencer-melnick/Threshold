// Copyright (c) 2020 Spencer Melnick

#include "AnimNotifyState_Tag.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"

void UAnimNotifyState_Tag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(MeshComp);

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AddLooseGameplayTags(AppliedTags);
}

void UAnimNotifyState_Tag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(MeshComp);

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->RemoveLooseGameplayTags(AppliedTags);
}


UTHAbilitySystemComponent* UAnimNotifyState_Tag::GetAbilitySystemComponent(USkeletalMeshComponent* MeshComponent)
{
	if (!MeshComponent)
	{
		return nullptr;
	}

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComponent->GetOwner());

	if (!BaseCharacter)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot fire AnimNotifyState_Tag with mesh component %s - component "
			"does not have a valid BaseCharacter owner"), *GetNameSafe(MeshComponent))
		return nullptr;
	}

	UTHAbilitySystemComponent* AbilitySystemComponent = BaseCharacter->GetAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot fire AnimNotifyState_Tag with character %s - character "
            "does not have a valid AbilitySystemComponent"), *GetNameSafe(BaseCharacter))
		return nullptr;
	}

	return AbilitySystemComponent;
}
