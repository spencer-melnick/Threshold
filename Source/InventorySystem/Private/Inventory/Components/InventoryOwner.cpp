// Copyright (c) 2020 Spencer Melnick

#include "Inventory/Components/InventoryOwner.h"
#include "Inventory/Components/InventoryComponent.h"


IInventoryOwner::FAdditionResult IInventoryOwner::AddItem(const FInventoryItem& NewItem)
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		return FAdditionResult(0, FInventoryArrayHandle());
	}

	return InventoryComponent->AddItem(NewItem);
}
