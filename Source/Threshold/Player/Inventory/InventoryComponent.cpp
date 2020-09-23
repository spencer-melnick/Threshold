// Copyright (c) 2020 Spencer Melnick

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"


// UInventoryComponent

UInventoryComponent::UInventoryComponent()
{
	
}



// Network replication

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryArray);
}

void UInventoryComponent::OnRep_Inventory()
{
	
}

