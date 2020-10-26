// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThresholdUI/Interfaces/PlayerWidget.h"
#include "ThresholdUI/Interfaces/SelectableWidget.h"
#include "InventoryMenu.generated.h"



// Forward declarations

class UInventoryGrid;
class UPreviewWidget;
class URichTextBlock;
struct FInventoryArrayHandle;



/**
 * Main inventory menu widget
 */
UCLASS()
class UInventoryMenu : public UUserWidget, public IPlayerWidgetInterface, public ISelectableWidget
{
	GENERATED_BODY()

public:

	// User widget overrides

	virtual void NativeOnInitialized() override;
	

	// Player widget overrides

	virtual void OnPlayerStateInitialized() override;


	// Selectable widget overrides

	virtual FSelectableWidgetReference TrySelect(const ESelectionDirection FromSelectionDirection) override;
	virtual FSelectableWidgetReference GetAdjacentWidget(const ESelectionDirection InSelectionDirection) const override { return nullptr; }
	virtual FSelectableWidgetReference GetParentWidget() const override { return nullptr; }
	virtual void InitializeSelection(TScriptInterface<ISelectionController> Controller) override;


	// Display controls

	/**
	 * Displays the details of the selected inventory item (or clears the display if there is none)
	 */
	void DisplayItem(FInventoryArrayHandle ItemHandle);
	

private:

	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UInventoryGrid* GridWidget;

	UPROPERTY(meta=(BindWidget))
	UPreviewWidget* PreviewDisplay;
	
	UPROPERTY(meta=(BindWidget))
	URichTextBlock* ItemDescription;
	
};
