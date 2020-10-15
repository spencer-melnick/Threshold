// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryBlock.generated.h"



// Forward declarations

class UTextBlock;
class UPreviewWidget;
class UInventoryComponent;
struct FInventoryItem;



/**
 * Simple widget used for displaying a small window with information about an inventory item, usually as part of a grid
 */
UCLASS()
class THRESHOLDUI_API UInventoryBlock : public UUserWidget
{
	GENERATED_BODY()
	
public:


	// Inventory controls
	
	/**
	 * Assigns the item to display a specific index in an inventory
	 * @param InventoryComponent - Inventory to attach to
	 * @param Index - Index of the item in the inventory array
	 */
	UFUNCTION(BlueprintCallable, Category=InventoryBlock)
	void AssignToInventory(UInventoryComponent* InventoryComponent, int32 Index);

	/**
	 * Updates the display using the bound inventory component and index
	 */
	UFUNCTION(BlueprintCallable, Category=InventoryBlock)
	void UpdateDisplay();


	// Accessors

	UFUNCTION(BlueprintCallable, Category=InventoryBlock)
	UInventoryComponent* GetParentInventory() const { return ParentInventory; }

	UFUNCTION(BlueprintCallable, Category=InventoryBlock)
	int32 GetInventoryIndex() const { return InventoryIndex; }


protected:

	// Helper functions
	
	const FInventoryItem* GetInventoryItem() const;

	

private:

	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackDisplay;

	UPROPERTY(meta=(BindWidget))
	UPreviewWidget* PreviewDisplay;



	// Binding values

	UPROPERTY()
	UInventoryComponent* ParentInventory;

	UPROPERTY()
	int32 InventoryIndex;
};
