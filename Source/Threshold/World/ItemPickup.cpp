// Copyright (c) 2020 Spencer Melnick

#include "ItemPickup.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Threshold/Threshold.h"
#include "Threshold/Global/Subsystems/InteractionSubsystem.h"
#include "Threshold/Player/Inventory/InventoryOwner.h"
#include "Threshold/Player/Inventory/InventoryComponent.h"


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
	Super::BeginPlay();
	
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

	if (!ItemHelper)
	{
		return;
	}

	IInventoryOwner* InventoryOwner = Cast<IInventoryOwner>(Character);
	if (!InventoryOwner)
	{
		return;
	}

	InventoryOwner->GetInventoryComponent()->AddInventoryItem(ItemHelper->GetItem());
}

