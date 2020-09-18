// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.generated.h"


// Forward declarations

class ABaseCharacter;



/**
 * Interface for any actor that can be interacted with by a player. If the game is running on multiplayer, interactions
 * might be triggered by the client predictively, so any actor implementing this should be prepared to reverse changes
 * until the change is confirmed by the server
 */
UINTERFACE()
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

class IInteractiveObject
{
	GENERATED_BODY()

public:
	virtual bool CanInteract(ABaseCharacter* Instigator) const { return true; }
	virtual FVector GetInteractLocation() const = 0;
	virtual void AttachInteractionIndicator(AActor* Indicator);

	// Called when the server actually runs the interaction
	virtual void OnServerInteract(ABaseCharacter* Character) = 0;
};
