// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGrid.generated.h"



// Forward declarations

class UInventoryComponent;
class UInventoryBlock;
class UUniformGridPanel;
class UUniformGridSlot;



/**
 * Simple widget that constructs a grid of inventory block widgets
 */
UCLASS()
class THRESHOLDUI_API UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:

	UInventoryGrid(const FObjectInitializer& ObjectInitializer);
	

	// User widget overrides

	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;


	// Inventory controls

	/**
	 * Assigns this inventory grid to reference a specific inventory component, with an optional starting index offset.
	 * Updates displays of all inventory blocks
	 */
	void AssignInventoryComponent(UInventoryComponent* InventoryComponent, int32 InStartingInventoryIndex = 0);

	/**
	 * Updates all inventory blocks based on the bound inventory component and index
	 */
	void UpdateDisplay();


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
	 * will simply be cleared. Does not trigger an update of the sub block contents.
	 */
	void ConstructSubBlocks();

	/**
	* Binds all sub inventory blocks to an inventory component, and updates the displays
	*/
	void BindSubBlocks();

	/**
	 * Constructs sub inventory blocks, binds them to the inventory component (if one is assigned) and updates
	 * their displays
	 */
	void Reconstruct();


	// Sub-widgets

	UPROPERTY()
	TArray<UInventoryBlock*> SubBlocks;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* GridPanel;

	UPROPERTY()
	TArray<UUniformGridSlot*> GridSlots;


	// Parent inventory references
	
	UPROPERTY()
	UInventoryComponent* ParentInventory;

	int32 StartingInventoryIndex;
};
