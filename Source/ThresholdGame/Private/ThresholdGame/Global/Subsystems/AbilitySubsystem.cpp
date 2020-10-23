// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Global/Subsystems/AbilitySubsystem.h"
#include "AbilitySystemGlobals.h"



void UAbilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}

