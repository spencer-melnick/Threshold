// Copyright (c) 2020 Spencer Melnick

#include "ItemPickup.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Threshold/Threshold.h"


// Component name constants

FName AItemPickup::MeshComponentName(TEXT("MeshComponent"));



// AItemPickup

AItemPickup::AItemPickup()
{
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(MeshComponentName);
}



// Interactive object overrides

bool AItemPickup::CanInteract(ABaseCharacter* Character) const
{
	// Only interactive if not pending kill and visible
	return !IsPendingKill() && PendingPredictionKeys.Num() <= 0 && !bConfirmedServerInteraction;
}

void AItemPickup::OnClientInteract(ABaseCharacter* Character, FPredictionKey& PredictionKey)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s predicted interaction with %s"), *GetNameSafe(Character), *GetNameSafe(this))

	if (PendingPredictionKeys.Num() <= 0)
	{
		// If this is the first pending prediction, hide the actor
		HideObject();
	}

	// Track this attempted activation
	PendingPredictionKeys.Add(PredictionKey.Current);
}

void AItemPickup::OnClientInteractionRejected(ABaseCharacter* Character, FPredictionKey& PredictionKey)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s rejected interaction with %s"), *GetNameSafe(Character), *GetNameSafe(this))

	// Remove the attempted activation
	PendingPredictionKeys.Remove(PredictionKey.Current);

	if (bConfirmedServerInteraction && PendingPredictionKeys.Num() <= 0)
	{
		// If the server hasn't confirmed interaction and there are no pending predictions, revert to the visible state
		ShowObject();
	}
}

void AItemPickup::OnClientInteractionConfirmed(ABaseCharacter* Character, FPredictionKey& PredictionKey)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s confirmed interaction with %s"), *GetNameSafe(Character), *GetNameSafe(this))
	
	// If the server confirmed any predicted interaction, then this object should no longer be visible on the server
	bConfirmedServerInteraction = true;
	PendingPredictionKeys.Remove(PredictionKey.Current);
}

void AItemPickup::OnServerInteraction(ABaseCharacter* Character)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s interacted with with %s"), *GetNameSafe(Character), *GetNameSafe(this))

	// TODO: Add item to inventory here
}




// Helper functions

void AItemPickup::HideObject()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AItemPickup::ShowObject()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
