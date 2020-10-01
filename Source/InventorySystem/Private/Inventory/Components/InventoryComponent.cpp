// Copyright (c) 2020 Spencer Melnick

#include "Inventory/Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// UInventoryComponent

UInventoryComponent::UInventoryComponent()
{
	// Should be replicated by default
	SetIsReplicatedByDefault(true);
}



// Engine overrides

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryArray);
}



// Inventory access

int32 UInventoryComponent::AddItem(const FInventoryItem& NewItem)
{
	if (!NewItem.IsValid())
	{
		return 0;
	}

	// Place a copy of our item in the inventory and mark it as dirty for replication
	FInventoryItem& ItemCopy = InventoryArray.Items.Emplace_GetRef(NewItem);
	InventoryArray.MarkItemDirty(ItemCopy);
	return 1;
}



// Network replication

void UInventoryComponent::OnRep_InventoryArray()
{
	
}

