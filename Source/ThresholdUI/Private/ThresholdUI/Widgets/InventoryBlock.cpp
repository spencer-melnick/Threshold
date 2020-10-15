// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "ThresholdUI/Widgets/PreviewWidget.h"
#include "Components/TextBlock.h"
#include "Inventory/Components/InventoryComponent.h"



// UInventoryBlock

// Inventory controls

void UInventoryBlock::AssignToInventory(UInventoryComponent* InventoryComponent, int32 Index)
{
	ParentInventory = InventoryComponent;
	InventoryIndex = Index;

	UpdateDisplay();
}

void UInventoryBlock::UpdateDisplay()
{
	#if WITH_EDITOR
		if (!StackDisplay || !PreviewDisplay)
		{
			return;
		}
	#endif
	
	const FInventoryItem* InventoryItem = GetInventoryItem();

	if (!InventoryItem)
	{
		// Clear display
		StackDisplay->SetText(FText());
		PreviewDisplay->ClearPreviewActorClass();
	}
	else
	{
		if (InventoryItem->AllowsStacking())
		{
			StackDisplay->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
		}
		else
		{
			StackDisplay->SetText(FText::AsCultureInvariant(TEXT("Test")));
		}

		PreviewDisplay->SetPreviewActorClass(InventoryItem->GetPreviewActorClass());
	}
}



// Helper functions

const FInventoryItem* UInventoryBlock::GetInventoryItem() const
{
	if (!ParentInventory || InventoryIndex < 0)
	{
		return nullptr;
	}

	const TArray<FInventoryItem>& InventoryArray = ParentInventory->GetArray();

	if (InventoryIndex >= InventoryArray.Num())
	{
		return nullptr;
	}

	return &InventoryArray[InventoryIndex];
}

