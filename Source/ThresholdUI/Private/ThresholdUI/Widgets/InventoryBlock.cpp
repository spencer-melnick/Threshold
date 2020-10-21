// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Inventory/InventoryArray.h"



// UInventoryBlock

// Inventory controls

void UInventoryBlock::DisplayItem(FInventoryArrayHandle InItemHandle)
{
	#if WITH_EDITOR
		if (!StackDisplay || !ThumbnailDisplay)
		{
			return;
		}
	#endif
	
	ItemHandle = InItemHandle;
	FInventoryItem* InventoryItem = ItemHandle.Get();

	if (!InventoryItem)
	{
		ClearDisplay();
		return;
	}

	StackDisplay->SetText(GetStackText(InventoryItem));
	SetBrushTexture(InventoryItem->GetThumbnailImage());
}

void UInventoryBlock::ClearDisplay()
{
	#if WITH_EDITOR
		if (!StackDisplay || !ThumbnailDisplay)
		{
			return;
		}
	#endif
	
	// Clear the item handle
	ItemHandle.Clear();

	// Clear the display
	StackDisplay->SetText(GetStackText(nullptr));
	ClearBrush();
}



// Helper functions

FText UInventoryBlock::GetStackText(FInventoryItem* InventoryItem)
{
	if (!InventoryItem || !InventoryItem->AllowsStacking())
	{
		return FText();
	}

	return FText::AsNumber(InventoryItem->GetStackCount());
}

void UInventoryBlock::ClearBrush()
{
	ThumbnailDisplay->SetBrush(FSlateNoResource());
}

void UInventoryBlock::SetBrushTexture(TSoftObjectPtr<UTexture2D> Texture)
{
	if (Texture.IsNull())
	{
		ClearBrush();
		return;
	}

	ThumbnailDisplay->SetBrushFromSoftTexture(Texture);
}

