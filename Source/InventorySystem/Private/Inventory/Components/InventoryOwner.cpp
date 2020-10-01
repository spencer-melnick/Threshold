// Copyright (c) 2020 Spencer Melnick

#include "Inventory/Components/InventoryOwner.h"
#include "Inventory/Components/InventoryComponent.h"


int32 IInventoryOwner::AddItem(const FInventoryItem& NewItem)
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		return 0;
	}

	return InventoryComponent->AddItem(NewItem);
}
