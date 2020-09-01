// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCue_WeaponHit.generated.h"



// Forward declarations

class UNiagaraSystem;



/**
 * GameplayCue used to spawn projectiles and sound effects when a weapon hits
 */
UCLASS(Abstract)
class UGameplayCue_WeaponHit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	// GameplayCue overrides

	virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;



	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	UNiagaraSystem* ParticleSystem;
};
