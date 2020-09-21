// Copyright (c) 2020 Spencer Melnick

#include "THPlayerState.h"
#include "Threshold/Player/Inventory/InventoryComponent.h"


// ATHPlayerState

ATHPlayerState::ATHPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(InventoryComponentName);
}



// Component Name constants

FName ATHPlayerState::InventoryComponentName(TEXT("InventoryComponent"));



// Engine overrides

void ATHPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

