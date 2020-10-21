// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "HUDControl.generated.h"



/**
* Enum describing status of player HUD
*/
UENUM(BlueprintType)
enum class EPlayerHUDStatus : uint8
{
	// Player menu is hidden, and the main view is the 3D world view
	WorldView,
	
    // Player menu is active, worldview is unfocused
    PlayerMenuActive
};



/**
 * Interface for player HUD class to prevent circular modular dependencies
 */
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UHUDControl : public UInterface
{
	GENERATED_BODY()
};

class IHUDControl
{
	GENERATED_BODY()

public:
	/**
	 * Sets the player HUD status, hiding or showing any HUD widgets as necessary
	 * @param NewStatus - The new HUD status
	 */
	UFUNCTION(BlueprintCallable, Category=HUDControl)
    virtual void SetStatus(const EPlayerHUDStatus NewStatus) = 0;

	UFUNCTION(BlueprintCallable, Category=HUDControl)
	virtual EPlayerHUDStatus GetStatus() const = 0;

	/**
	 * Used to check whether or not the player should have control over their character, based on the HUD status.
	 * Useful for disabling character control when the menu is active
	 * @return True if the player should be able to control their character, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category=HUDControl)
	virtual bool ShouldEnableCharacterControl() const = 0;
};
