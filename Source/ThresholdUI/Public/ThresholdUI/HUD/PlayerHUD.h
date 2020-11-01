// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ThresholdGame/Player/HUDControl.h"
#include "PlayerHUD.generated.h"


// Forward declarations

class UPlayerMenuWidget;
class UGameOverlay;
 
 
 
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



	// HUD control overrides

	/**
	 * Sets the player HUD status, hiding or showing any HUD widgets as necessary
	 * @param NewStatus - The new HUD status
	 */
	UFUNCTION(BlueprintCallable, Category=PlayerHUD)
	virtual void SetStatus(const EPlayerHUDStatus NewStatus) override;

	virtual EPlayerHUDStatus GetStatus() const override { return Status; }
	virtual bool ShouldEnableCharacterControl() const override;
	virtual void ShowItemPickupNotification(FInventoryItem& Item) override;
	virtual void OnPlayerStateInitialized() override;



	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPlayerMenuWidget> PlayerMenuClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UGameOverlay> GameOverlayClass;


protected:

	// Helper functions
	
	void CreateWidgets();

	/**
	 * Creates a widget from the specified class and assigns it to the variable, displaying error messages if anything
	 * fails
	 * @param WidgetClass - Class of the widget to create
	 * @param WidgetVariable - Variable to store the create widget at
	 */
	template <typename WidgetType>
	void CreateWidgetFromClassChecked(TSubclassOf<WidgetType> WidgetClass, WidgetType*& WidgetVariable);
	


private:
	// Widgets

	UPROPERTY()
	UPlayerMenuWidget* PlayerMenuWidget;

	UPROPERTY()
	UGameOverlay* GameOverlay;



	// HUD status

	UPROPERTY(EditDefaultsOnly, Category=PlayerHUD, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	EPlayerHUDStatus Status;
};
