// Copyright (c) 2020 Spencer Melnick

#include "GameplayCue_WeaponHit.h"
#include "NiagaraFunctionLibrary.h"
#include "Threshold/Threshold.h"


bool UGameplayCue_WeaponHit::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return EventType == EGameplayCueEvent::Executed;
}

void UGameplayCue_WeaponHit::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	check(EventType == EGameplayCueEvent::Executed);

	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	if (!HitResult)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("GameplayCue_WeaponHit triggered on %s with no hit result - effect will not be applied"), *GameplayCueTag.GetTagName().ToString())
		return;
	}

	if (ParticleSystem)
	{
		const FVector SwingNormal = (HitResult->TraceEnd - HitResult->TraceStart).GetSafeNormal();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, HitResult->Location, SwingNormal.Rotation());
	}
}

