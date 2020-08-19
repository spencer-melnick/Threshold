// Copyright © 2020 Spencer Melnick

#include "AbilitySubsystem.h"
#include "AbilitySystemGlobals.h"



void UAbilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}

