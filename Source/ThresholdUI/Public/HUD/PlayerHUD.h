// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"


// Forward declarations

class UPlayerMenuWidget;



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
 * Main HUD class for players, includes menus and screen overlays
 */
UCLASS()
class THRESHOLDUI_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();


	// Engine overrides

	virtual void BeginPlay() override;



	// Accessors

	/**
	 * Sets the player HUD status, hiding or showing any HUD widgets as necessary
	 * @param NewStatus - The new HUD status
	 */
	UFUNCTION(BlueprintCallable, Category=PlayerHUD)
	void SetStatus(const EPlayerHUDStatus NewStatus);

	EPlayerHUDStatus GetStatus() const { return Status; }

	/**
	 * Used to check whether or not the player should have control over their character, based on the HUD status.
	 * Useful for disabling character control when the menu is active
	 * @return True if the player should be able to control their character, false otherwise
	 */
	bool ShouldEnableCharacterControl() const;



	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPlayerMenuWidget> PlayerMenuClass;


protected:

	// Helper functions
	
	void CreateWidgets();

	/*
	 * Hides the specified widget, disabling input, and checking for invalid widgets
	 * @param Widget - Widget to hide
	 */
	static void HideWidgetChecked(UUserWidget* Widget);

	/*
	 * Shows the specified widget, enabling input, and checking for invalid widgets
	 * @param Widget - Widget to show
	*/
	static void ShowWidgetChecked(UUserWidget* Widget);
	


private:
	// Widgets

	UPROPERTY()
	UPlayerMenuWidget* PlayerMenuWidget;



	// HUD status

	UPROPERTY(EditDefaultsOnly, Category=PlayerHUD, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	EPlayerHUDStatus Status;
};
