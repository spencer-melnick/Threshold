// Copyright (c) 2020 Spencer Melnick

#include "AbilityFunctionLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

FGameplayEventData UAbilityFunctionLibrary::CreateGameplayEvent(AActor* Instigator, AActor* Target, FGameplayTag EventTag)
{
	FGameplayEventData EventData;

	EventData.Instigator = Instigator;
	EventData.Target = Target;
	EventData.EventTag = EventTag;

	IAbilitySystemInterface* InstigatorInterface = Cast<IAbilitySystemInterface>(Instigator);
	if (InstigatorInterface)
	{
		UAbilitySystemComponent* InstigatorASC = InstigatorInterface->GetAbilitySystemComponent();
		if (InstigatorASC)
		{
			InstigatorASC->GetOwnedGameplayTags(EventData.InstigatorTags);
		}
	}

	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Target);
	if (TargetInterface)
	{
		UAbilitySystemComponent* TargetASC = TargetInterface->GetAbilitySystemComponent();
		if (TargetASC)
		{
			TargetASC->GetOwnedGameplayTags(EventData.InstigatorTags);
		}
	}

	return EventData;
}

FGameplayCueParameters UAbilityFunctionLibrary::CreateGameplayCue(AActor* Instigator, AActor* EffectCauser, AActor* Target)
{
	FGameplayCueParameters CueParameters;

	CueParameters.Instigator = Instigator;
	CueParameters.EffectCauser = EffectCauser;

	IAbilitySystemInterface* InstigatorInterface = Cast<IAbilitySystemInterface>(Instigator);
	if (InstigatorInterface)
	{
		UAbilitySystemComponent* InstigatorASC = InstigatorInterface->GetAbilitySystemComponent();
		if (InstigatorASC)
		{
			InstigatorASC->GetOwnedGameplayTags(CueParameters.AggregatedSourceTags);
		}
	}

	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Target);
	if (TargetInterface)
	{
		UAbilitySystemComponent* TargetASC = TargetInterface->GetAbilitySystemComponent();
		if (TargetASC)
		{
			TargetASC->GetOwnedGameplayTags(CueParameters.AggregatedTargetTags);
		}
	}

	return CueParameters;
}

