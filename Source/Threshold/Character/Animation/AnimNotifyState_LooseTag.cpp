// Copyright (c) 2020 Spencer Melnick

#include "AnimNotifyState_LooseTag.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Abilities/THAbilitySystemComponent.h"



// UAnimNotifyState_LooseTag

void UAnimNotifyState_LooseTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(MeshComp);

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AddLooseGameplayTags(AppliedTags);
}

void UAnimNotifyState_LooseTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(MeshComp);

	if (!AbilitySystemComponent)
	{
		return;
	}

	const bool bPredictedElsewhere = AbilitySystemComponent->AbilityActorInfo->IsNetAuthority() && !AbilitySystemComponent->AbilityActorInfo->IsLocallyControlled();

	/* if (!bExtendTagOnServer || !bPredictedElsewhere)
	{
		AbilitySystemComponent->RemoveLooseGameplayTags(AppliedTags);
	}
	else */
	{
		// Extend the tag on the server
		const FGameplayTagContainer AppliedTagsCopy = AppliedTags;
		FTimerHandle TimerHandle;
		const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([AbilitySystemComponent, AppliedTagsCopy]
		{
			if (IsValid(AbilitySystemComponent))
			{
				AbilitySystemComponent->RemoveLooseGameplayTags(AppliedTagsCopy);
			}
		});
		const float TagExtensionTime = IConsoleManager::Get().FindConsoleVariable(TEXT("th.TagExtensionTime"))->GetFloat();
		MeshComp->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, TagExtensionTime, false);
	}
}


UAbilitySystemComponent* UAnimNotifyState_LooseTag::GetAbilitySystemComponent(USkeletalMeshComponent* MeshComponent)
{
	if (!MeshComponent)
	{
		return nullptr;
	}

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComponent->GetOwner());

	if (!BaseCharacter)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot fire AnimNotifyState_LooseTag with mesh component %s - component "
			"does not have a valid BaseCharacter owner"), *GetNameSafe(MeshComponent))
		return nullptr;
	}

	UAbilitySystemComponent* AbilitySystemComponent = BaseCharacter->GetAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("Cannot fire AnimNotifyState_LooseTag with character %s - character "
            "does not have a valid AbilitySystemComponent"), *GetNameSafe(BaseCharacter))
		return nullptr;
	}

	return AbilitySystemComponent;
}

FString UAnimNotifyState_LooseTag::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("LooseTag: {0}"), {AppliedTags.ToStringSimple()});
}
