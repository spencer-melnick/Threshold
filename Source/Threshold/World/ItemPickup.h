// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "ItemPickup.generated.h"


// Forward declarations

class UMeshComponent;


/**
 * Generic actor for picking up inventory objects
 */
UCLASS()
class AItemPickup : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	AItemPickup();


	// Interactive object overrides

	virtual bool CanInteract(ABaseCharacter* Character) const override;
	virtual void OnClientInteract(ABaseCharacter* Character, FPredictionKey& PredictionKey) override;
	virtual void OnClientInteractionRejected(ABaseCharacter* Character, FPredictionKey& PredictionKey) override;
	virtual void OnClientInteractionConfirmed(ABaseCharacter* Character, FPredictionKey& PredictionKey) override;
	virtual void OnServerInteraction(ABaseCharacter* Character) override;


	
	// Component names

	static FName MeshComponentName;


protected:
	// Helper functions

	void HideObject();
	void ShowObject();
	
	
	// Array tracking client side interactions that haven't been confirmed yet
	TArray<int16> PendingPredictionKeys;

	bool bConfirmedServerInteraction = false;
};
