// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryArray.h"
#include "ThresholdUI/Interfaces/SelectableWidget.h"
#include "InventoryBlock.generated.h"



// Forward declarations

class UTextBlock;
class UImage;
class UInventoryComponent;
struct FInventoryItem;
class UInventoryGrid;



/**
 * Simple widget used for displaying a small window with information about an inventory item, usually as part of a grid
 */
UCLASS()
class THRESHOLDUI_API UInventoryBlock : public UUserWidget, public ISelectableWidget
{
	GENERATED_BODY()
	
public:

	// Widget overrides

	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	

	// Inventory controls
	
	/**
	 * Displays a specific item in an inventory
	 * @param InItemHandle - Handle to the item to be displayed - will clear display if invalid
	 */
	void DisplayItem(FInventoryArrayHandle InItemHandle);

	/**
	 * Clears the item display and item handle
	 */
	void ClearDisplay();

	/**
	 * Assigns the parent grid of this inventory block
	 */
	void SetParentGrid(UInventoryGrid* InParentGrid, FIntPoint InGridCell)
	{
		ParentGrid = InParentGrid;
		GridCell = InGridCell;
	}


	// Selectable widget overrides

	virtual FSelectableWidgetReference TrySelect(const ESelectionDirection FromSelectionDirection) override;
	virtual FSelectableWidgetReference GetAdjacentWidget(const ESelectionDirection InSelectionDirection) const override;
	virtual FSelectableWidgetReference GetParentWidget() const override;
	virtual void OnDeselected() override;
	virtual void InitializeSelection(TScriptInterface<ISelectionController> Controller) override;


	// Accessors

	FInventoryArrayHandle GetItemHandle() const { return ItemHandle; }
	UInventoryGrid* GetParentGrid() const { return ParentGrid; }
	FIntPoint GetGridCell() const { return GridCell; }


	// Blueprint events

	UFUNCTION(BlueprintImplementableEvent, Category=InventoryBlock, meta=(DisplayName="OnSelected"))
	void Blueprint_OnSelected(bool bSelected);
	

	
	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FName TextureParameter;


protected:

	// Helper functions
	
	static FText GetStackText(FInventoryItem* InventoryItem);

	void SetBrushTexture(TSoftObjectPtr<UTexture2D> Texture);
	

private:

	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackDisplay;

	UPROPERTY(meta=(BindWidget))
	UImage* ThumbnailDisplay;


	// Binding values

	FInventoryArrayHandle ItemHandle;


	// Parent widgets
	
	UPROPERTY()
	UInventoryGrid* ParentGrid;

	FIntPoint GridCell;

	UPROPERTY()
	TScriptInterface<ISelectionController> SelectionController;
};
