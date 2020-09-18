// Copyright (c) 2020 Spencer Melnick

#include "ItemPickup.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Threshold/Threshold.h"
#include "Threshold/Global/Subsystems/InteractionSubsystem.h"


// Component name constants

FName AItemPickup::MeshComponentName(TEXT("MeshComponent"));



// AItemPickup

AItemPickup::AItemPickup()
{
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(MeshComponentName);
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


void AItemPickup::OnServerInteract(ABaseCharacter* Character)
{
	UE_LOG(LogThresholdGeneral, Display, TEXT("%s picked up %s"), *GetNameSafe(Character), *GetNameSafe(this))
}

