// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Character/Animation/AnimNotify_GameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, {});
}

FString UAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("GameplayEvent: {0}"), {EventTag.ToString()});
}

