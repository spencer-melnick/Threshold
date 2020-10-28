// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryMenu.h"
#include "ThresholdUI/Widgets/InventoryGrid.h"
#include "ThresholdUI/Widgets/PreviewWidget.h"
#include "Components/RichTextBlock.h"
#include "Inventory/InventoryArray.h"



// UInventoryMenu

// User widget overrides

void UInventoryMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	#if WITH_EDITOR
		if (!GridWidget)
		{
			return;
		}
	#endif

	GridWidget->InventoryGridSelectedDelegate.BindUObject(this, &UInventoryMenu::DisplayItem);
}




// Player widget overrides

void UInventoryMenu::OnPlayerStateInitialized()
{
	GridWidget->OnPlayerStateInitialized();
}



// Selectable widget overrides

FSelectableWidgetReference UInventoryMenu::TrySelect(const ESelectionDirection FromSelectionDirection)
{
	return GridWidget->TrySelect(FromSelectionDirection);
}

void UInventoryMenu::InitializeSelection(TScriptInterface<ISelectionController> Controller)
{
	GridWidget->InitializeSelection(Controller);
}



// Display controls

void UInventoryMenu::DisplayItem(FInventoryArrayHandle ItemHandle)
{
	#if WITH_EDITOR
		if (!PreviewDisplay || !ItemDescription)
		{
			return;
		}
	#endif
	
	FInventoryItem* Item = ItemHandle.Get();

	if (!Item)
	{
		PreviewDisplay->ClearPreviewActorClass();
		ItemDescription->SetText(FText());
	}
	else
	{
		PreviewDisplay->SetPreviewActorClass(Item->GetPreviewActorClass());
		ItemDescription->SetText(Item->GetDescription());
	}
}

