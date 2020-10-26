// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PlayerWidget.generated.h"



/**
 * Interface used for common player widgets
 */
UINTERFACE()
class UPlayerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class IPlayerWidgetInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called whenever the player state is initialized, or after all the starting widgets are attached to the player HUD
	 * (if the player state was created before the HUD)
	 */
	virtual void OnPlayerStateInitialized() { };
};
