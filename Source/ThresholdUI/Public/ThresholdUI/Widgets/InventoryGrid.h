// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThresholdUI/Interfaces/PlayerWidget.h"
#include "InventoryGrid.generated.h"



// Forward declarations

class UInventoryComponent;
class UInventoryBlock;
class UUniformGridPanel;
class UUniformGridSlot;
class UInventoryGrid;


// Delegate declarations

DECLARE_DELEGATE_OneParam(FInventoryMouseoverDelegate, UInventoryGrid*)



/**
 * Simple widget that constructs a grid of inventory block widgets
 */
UCLASS()
class THRESHOLDUI_API UInventoryGrid : public UUserWidget, public IPlayerWidgetInterface
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
};
