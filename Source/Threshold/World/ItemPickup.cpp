// Copyright (c) 2020 Spencer Melnick

#include "ItemPickup.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Threshold/Threshold.h"
#include "Threshold/Character/Player/PlayerCharacter.h"
#include "Threshold/Global/Subsystems/InteractionSubsystem.h"
#include "Threshold/Global/Subsystems/InventorySubsystem.h"
#include "Threshold/Player/Inventory/InventoryComponent.h"
#include "Threshold/Player/Inventory/InventoryItem.h"
#include "Threshold/Player/Inventory/Items/InventoryTableItem.h"


// Component name constants

FName AItemPickup::MeshComponentName(TEXT("MeshComponent"));



// AItemPickup

AItemPickup::AItemPickup()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(MeshComponentName);
	RootComponent = MeshComponent;
}



// Engine overrides

void AItemPickup::BeginPlay()
{
	GetWorld()->GetSubsystem<UInteractionSubsystem>()->RegisterObject(this);
}

void AItemPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UInteractionSubsystem* InteractionSubsystem = GetWorld()->GetSubsystem<UInteractionSubsystem>();

	if (InteractionSubsystem)
	{
		InteractionSubsystem->UnregisterObject(this);
	}
}




// Interactive object overrides

bool AItemPickup::CanInteract(ABaseCharacter* Character) const
{
	return true;
}

FVector AItemPickup::GetInteractLocation() const
{
	return GetActorLocation();
}

void AItemPickup::AttachInteractionIndicator(AActor* Indicator)
{
	if (!Indicator)
	{
		return;
	}

	Indicator->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}


void AItemPickup::OnServerInteract(ABaseCharacter* Character)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s picked up %s"), *GetNameSafe(Character), *GetNameSafe(this))

	// Load the item from the inventory subsystem
	UInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UInventorySubsystem>();
	check(InventorySubsystem);
	const TScriptInterface<IInventoryItem> InventoryItem = InventorySubsystem->GetItemByName(InventoryItemName);
	if (!InventoryItem)
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("%s is not a valid item name"), *InventoryItemName.ToString())
		return;
	}

	// Get the inventory component
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (!PlayerCharacter || !PlayerCharacter->GetInventoryComponent())
	{
		UE_LOG(LogThresholdGeneral, Warning, TEXT("%s does not have an inventory component"), *GetNameSafe(Character))
		return;
	}

	const int32 CountAdded = PlayerCharacter->GetInventoryComponent()->AddInventoryItem(InventoryItem, Count);

	if (bSinglePickup)
	{
		Count -= CountAdded;

		if (Count <= 0)
		{
			Destroy();
		}
	}
}

