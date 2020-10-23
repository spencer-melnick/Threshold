// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Player/THPlayerState.h"
#include "Inventory/Components/InventoryComponent.h"


// ATHPlayerState

ATHPlayerState::ATHPlayerState()
{
	NetUpdateFrequency = 10.f;
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(InventoryComponentName);
}



// Component Name constants

FName ATHPlayerState::InventoryComponentName(TEXT("InventoryComponent"));



// Engine overrides

void ATHPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

