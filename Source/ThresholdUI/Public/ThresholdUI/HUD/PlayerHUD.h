// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Threshold/Player/HUDControl.h"
#include "PlayerHUD.generated.h"


// Forward declarations

class UPlayerMenuWidget;
 
 
 
/**
 * Main HUD class for players, includes menus and screen overlays
 */
UCLASS()
class THRESHOLDUI_API APlayerHUD : public AHUD, public IHUDControl
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
	virtual void SetStatus(const EPlayerHUDStatus NewStatus) override;

	virtual EPlayerHUDStatus GetStatus() const override { return Status; }

	/**
	 * Used to check whether or not the player should have control over their character, based on the HUD status.
	 * Useful for disabling character control when the menu is active
	 * @return True if the player should be able to control their character, false otherwise
	 */
	virtual bool ShouldEnableCharacterControl() const override;



	// Initialization

	virtual void OnPlayerStateInitialized() override;



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
