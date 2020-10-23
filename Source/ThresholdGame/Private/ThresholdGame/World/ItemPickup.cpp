// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/World/ItemPickup.h"
#include "ThresholdGame/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "ThresholdGame.h"
#include "ThresholdGame/Global/Subsystems/InteractionSubsystem.h"
#include "Inventory/Components/InventoryOwner.h"
#include "Inventory/InventoryArray.h"


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
	UE_LOG(LogThresholdGame, Display, TEXT("%s picked up %s"), *GetNameSafe(Character), *GetNameSafe(this))

	IInventoryOwner* InventoryOwner = Cast<IInventoryOwner>(Character);

	if (!InventoryOwner || !InventoryItem.IsValid())
	{
		return;
	}

	InventoryOwner->AddItem(InventoryItem);
}

