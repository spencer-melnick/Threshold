// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryArray.h"
#include "InventoryBlock.generated.h"



// Forward declarations

class UTextBlock;
class UImage;
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
	 * Displays a specific item in an inventory
	 * @param InItemHandle - Handle to the item to be displayed - will clear display if invalid
	 */
	void DisplayItem(FInventoryArrayHandle InItemHandle);

	/**
	 * Clears the item display and item handle
	 */
	void ClearDisplay();


	// Accessors

	FInventoryArrayHandle GetItemHandle() const { return ItemHandle; }


	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FName TextureParameter;


protected:

	// Helper functions
	
	static FText GetStackText(FInventoryItem* InventoryItem);

	void ClearBrush();
	void SetBrushTexture(TSoftObjectPtr<UTexture2D> Texture);
	

private:

	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackDisplay;

	UPROPERTY(meta=(BindWidget))
	UImage* ThumbnailDisplay;


	// Binding values

	FInventoryArrayHandle ItemHandle;
};
