// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "Brushes/SlateImageBrush.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Inventory/InventoryArray.h"



// UInventoryBlock

// Widget overrides

void UInventoryBlock::NativeConstruct()
{
	Super::NativeConstruct();

	#if WITH_EDITOR
		if (!StackDisplay || !ThumbnailDisplay)
		{
			return;
		}
	#endif

	if (ThumbnailDisplay->Brush.GetDrawType() != ESlateBrushDrawType::Image)
	{
		// Create a new image brush and set it to be hidden
		ThumbnailDisplay->SetBrush(FSlateImageBrush(static_cast<UObject*>(nullptr), FVector2D::UnitVector));
		ThumbnailDisplay->SetVisibility(ESlateVisibility::Hidden);
	}
}



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
	SetBrushTexture(nullptr);
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

void UInventoryBlock::SetBrushTexture(TSoftObjectPtr<UTexture2D> Texture)
{
	if (Texture.IsNull())
	{
		ThumbnailDisplay->Brush.SetResourceObject(nullptr);
		ThumbnailDisplay->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	
	ThumbnailDisplay->SetBrushFromSoftTexture(Texture);
	ThumbnailDisplay->SetVisibility(ESlateVisibility::Visible);
}

