// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppExpressionWithoutSideEffects

#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "Brushes/SlateImageBrush.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Inventory/InventoryArray.h"
#include "ThresholdUI/Widgets/InventoryGrid.h"
#include "ThresholdUI/Interfaces/SelectionController.h"



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

void UInventoryBlock::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!SelectionController)
	{
		return;
	}

	// Tell the selection controller we should be selected
	SelectionController->TrySelectWidget(FSelectableWidgetReference(this));
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



// Selectable widget overrides

FSelectableWidgetReference UInventoryBlock::TrySelect(const ESelectionDirection FromSelectionDirection)
{
	return FSelectableWidgetReference(this);
}

FSelectableWidgetReference UInventoryBlock::GetAdjacentWidget(const ESelectionDirection InSelectionDirection) const
{
	if (!ParentGrid)
	{
		return nullptr;
	}

	return FSelectableWidgetReference(ParentGrid->GetAdjacentBlock(GridCell, InSelectionDirection));
}

FSelectableWidgetReference UInventoryBlock::GetParentWidget() const
{
	if (!ParentGrid)
	{
		return nullptr;
	}

	return FSelectableWidgetReference(ParentGrid);
}

void UInventoryBlock::OnSelected()
{
	if (ParentGrid)
	{
		ParentGrid->SetDisplayBlock(this);	
	}
	
	Blueprint_OnSelected(true);
}


void UInventoryBlock::OnDeselected()
{
	Blueprint_OnSelected(false);
}


void UInventoryBlock::InitializeSelection(TScriptInterface<ISelectionController> Controller)
{
	SelectionController = Controller;
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
