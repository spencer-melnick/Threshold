// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThresholdUI/Interfaces/PlayerWidget.h"
#include "ThresholdUI/Interfaces/SelectionController.h"
#include "PlayerMenu.generated.h"



// Forward declarations

class UInventoryMenu;



/**
 * Main class for the player menu that contains the inventory screen and player status
 */
UCLASS()
class THRESHOLDUI_API UPlayerMenuWidget : public UUserWidget, public IPlayerWidgetInterface, public ISelectionController
{
	GENERATED_BODY()
	
public:
	UPlayerMenuWidget(const FObjectInitializer& ObjectInitializer);


	// Engine overrides

	virtual void NativeConstruct() override;



	// Initialization
	
	virtual void OnPlayerStateInitialized() override;



	// Input binding

	void SetupInputComponent(UInputComponent* InInputComponent);
	void MoveCursor(ESelectionDirection Direction);


protected:

	// Selection controller overrides
	
	virtual FSelectableWidgetReference* GetSelectedWidgetPointer() override { return &SelectedWidget; }
	virtual const FSelectableWidgetReference* GetSelectedWidgetPointer() const override { return &SelectedWidget; }


private:

	// Track selected widget
	
	UPROPERTY()
	TScriptInterface<ISelectableWidget> SelectedWidget;


	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UInventoryMenu* InventoryMenu;
};
