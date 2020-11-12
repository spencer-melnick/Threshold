// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/World/ItemPickup.h"
#include "ThresholdGame/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "ThresholdGame.h"
#include "ThresholdGame/Global/Subsystems/InteractionSubsystem.h"
#include "Inventory/Components/InventoryOwner.h"
#include "Inventory/InventoryArray.h"
#include "ThresholdGame/Player/THPlayerController.h"


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
	const IInventoryOwner::FAdditionResult AdditionResult = InventoryOwner->AddItem(InventoryItem);

	if (AdditionResult.Key > 0)
	{
		// Here, addition result key is the count of items added
		// Greater than 0 means that addition was successful and we should try to show a notification
		ATHPlayerController* PlayerController = Character->GetController<ATHPlayerController>();
		if (PlayerController)
		{
			// Make a copy of our item, but set its stack size to the count the player actually received if possible
			FInventoryItem ItemCopy = InventoryItem;
			if (ItemCopy.AllowsDuplicates())
			{
				ItemCopy.SetStackCount(AdditionResult.Key);
			}
			PlayerController->ClientShowItemPickupNotification(ItemCopy);
		}
	}
}

