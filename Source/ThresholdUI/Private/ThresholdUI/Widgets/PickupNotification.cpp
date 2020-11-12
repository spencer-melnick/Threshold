// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/PickupNotification.h"
#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "Inventory/InventoryItem.h"
#include "Components/RichTextBlock.h"


// UPickupNotification

// Display controls

void UPickupNotification::SetDisplayedItem(FInventoryItem& Item)
{
	#if WITH_EDITOR
		if (!ItemDisplay || !TextDisplay)
		{
			return;
		}
	#endif

	if (!Item.IsValid())
	{
		ItemDisplay->ClearDisplay();
		TextDisplay->SetText(FText());
	}
	else
	{
		UpdateFormatText();
		ItemDisplay->DisplayItem(Item);
		TextDisplay->SetText(FText::Format(NotificationText, {{TEXT("ItemName"), Item.GetName()}}));
	}
}



// Helper functions

void UPickupNotification::UpdateFormatText()
{
	if (!NotificationText.IdenticalTo(NotificationTextFormat.GetSourceText()))
	{
		// Check if the notification text matches the format text and re-compile the format if needed
		NotificationTextFormat = NotificationText;
	}
}

