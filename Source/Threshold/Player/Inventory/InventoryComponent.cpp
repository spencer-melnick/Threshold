// Copyright (c) 2020 Spencer Melnick

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Threshold/Threshold.h"


// UInventoryComponent

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}



// Inventory access

int32 UInventoryComponent::AddInventoryItem(FInventoryItem* Item)
{
	if (!Item)
	{
		return 0;
	}
	
	// Store a copy of the item in our inventory and mark our array for replication
	FInventoryItemHandle& HandleRef = InventoryArray.Items.Emplace_GetRef(Item->Copy());
	InventoryArray.MarkItemDirty(HandleRef);
	return 1;
}




// Network replication

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryArray);
}

void UInventoryComponent::OnRep_Inventory()
{
	for (FInventoryItemHandle& ItemHandle : InventoryArray.Items)
	{
		if (ItemHandle.ItemPointer.IsValid())
		{
			UE_LOG(LogThresholdGeneral, Display, TEXT("Item has type: %s"), *GetNameSafe(ItemHandle->GetScriptStruct()))
		}
		else
		{
			UE_LOG(LogThresholdGeneral, Display, TEXT("Item is invalid"))
		}
	}
}

