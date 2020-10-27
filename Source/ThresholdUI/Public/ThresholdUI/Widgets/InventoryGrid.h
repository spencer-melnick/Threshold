// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThresholdUI/Interfaces/PlayerWidget.h"
#include "ThresholdUI/Interfaces/SelectableWidget.h"
#include "InventoryGrid.generated.h"



// Forward declarations

class UInventoryComponent;
class UInventoryBlock;
class UUniformGridPanel;
class UUniformGridSlot;
class UInventoryGrid;
struct FInventoryArrayHandle;



// Delegate declarations

DECLARE_DELEGATE_OneParam(FInventoryGridSelectedDelegate, FInventoryArrayHandle);



/**
 * Simple widget that constructs a grid of inventory block widgets
 */
UCLASS()
class THRESHOLDUI_API UInventoryGrid : public UUserWidget, public IPlayerWidgetInterface, public ISelectableWidget
{
	GENERATED_BODY()

public:

	UInventoryGrid(const FObjectInitializer& ObjectInitializer);
	

	// User widget overrides

	virtual void NativeOnInitialized() override;
	virtual void SynchronizeProperties() override;


	// Inventory controls

	/**
	 * Assigns this inventory grid to reference a specific inventory component.
	 * Does not update displays of all inventory blocks
	 */
	void AssignInventoryComponent(UInventoryComponent* InInventoryComponent);

	/**
	 * Updates all inventory blocks
	 */
	UFUNCTION()
	void UpdateDisplay();


	// Initialization

	virtual void OnPlayerStateInitialized() override;


	// Navigation

	/**
	* Returns the cell coordinates adjacent to a specific cell, in a direction, or (-1, -1) if there is no adjacent cell
	*/
	FIntPoint GetAdjacentCell(const FIntPoint Cell, const ESelectionDirection Direction) const;

	/**
	 * Returns the inventory block adjacent to a specific cell, in a direction, or null if there is no adjacent block
	 */
	UInventoryBlock* GetAdjacentBlock(const FIntPoint Cell, const ESelectionDirection Direction) const { return GetBlockFromCell(GetAdjacentCell(Cell, Direction)); }

	/**
	* Returns the inventory block at the specific cell (if any)
	*/
	UInventoryBlock* GetBlockFromCell(FIntPoint Cell) const;

	/**
	 * Sets the default selected cell
	 */
	void SetSelectedCell(FIntPoint NewCell);

	/**
	 * Tries to set the selected block as the active detail display
	 */
	void SetDisplayBlock(UInventoryBlock* SelectedBlock);

	FIntPoint GetSelectedCell() const { return SelectedCell; }

	FInventoryArrayHandle GetSelectedItem() const;

	/**
	 * Checks if the cell is within the range of the grid size
	 */
	bool IsCellValid(FIntPoint Cell) const;


	// Selectable widget overrides

	virtual FSelectableWidgetReference TrySelect(const ESelectionDirection FromSelectionDirection) override;
	virtual FSelectableWidgetReference GetAdjacentWidget(const ESelectionDirection InSelectionDirection) const override { return nullptr; }
	virtual FSelectableWidgetReference GetParentWidget() const override { return nullptr; }
	virtual void InitializeSelection(TScriptInterface<ISelectionController> Controller) override;



	// Delegates

	FInventoryGridSelectedDelegate InventoryGridSelectedDelegate;
	


	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InventoryGrid)
	TSubclassOf<UInventoryBlock> InventoryBlockClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InventoryGrid)
	FIntPoint GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InventoryGrid)
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InventoryGrid)
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;


protected:

	// Helper functions

	/**
	 * Clears all sub inventory blocks and recreates them. If the grid size is negative in either dimension, the grid
	 * will simply be cleared. Does not trigger an update of the sub block contents. Skips if the grid size hasn't changed
	 */
	void ConstructSubBlocks();

	/**
	 * Constructs sub inventory blocks and updates their displays
	 */
	void Reconstruct();
	


	// Sub-widgets

	UPROPERTY()
	TArray<UInventoryBlock*> SubBlocks;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* GridPanel;

	UPROPERTY()
	TArray<UUniformGridSlot*> GridSlots;


	// Inventory reference
	
	UPROPERTY()
	UInventoryComponent* InventoryComponent;


private:

	FIntPoint PreviousGridSize = FIntPoint::ZeroValue;
	FIntPoint SelectedCell = FIntPoint::ZeroValue;

	UPROPERTY()
	TScriptInterface<ISelectionController> SelectionController;
};
