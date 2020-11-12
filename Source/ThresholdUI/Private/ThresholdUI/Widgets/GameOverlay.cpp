// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/GameOverlay.h"
#include "ThresholdUI/Widgets/NotificationSlot.h"
#include "ThresholdUI/Widgets/PickupNotification.h"
#include "ThresholdUI.h"
#include "Inventory/InventoryItem.h"



// UGameOverlay

void UGameOverlay::ShowItemPickupNotification(FInventoryItem& Item)
{
	CHECK_WIDGET_STATEMENT(PickupNotificationSlot)

	if (!PickupNotificationClass)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("GameOverlay::ShowItemPickupNotification failed on %s - no valid pickup notification class"),
			*GetNameSafe(this))
		return;
	}

	UPickupNotification* NewPickupNotification = CreateWidget<UPickupNotification>(PickupNotificationSlot, PickupNotificationClass);
	NewPickupNotification->SetDisplayedItem(Item);
	PickupNotificationSlot->AddChildNotification(NewPickupNotification);
}

