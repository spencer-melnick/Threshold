// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/GameOverlay.h"
#include "ThresholdUI/Widgets/PickupNotification.h"
#include "Inventory/InventoryItem.h"



// UGameOverlay

void UGameOverlay::ShowItemPickupNotification(FInventoryItem& Item)
{
	#if WITH_EDITOR
		if (!PickupDisplay)
		{
			return;
		}
	#endif

	PickupDisplay->ShowItemPickupNotification(Item);
}

