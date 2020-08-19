// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AbilitySubsystem.generated.h"

/**
 * This is a simple engine subsystem that initializes the ability system
 * globals. Ability system globals need to be initialized for the system
 * to work properly. This subsystem doesn't do much else currently.
 */
UCLASS()
class UAbilitySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
