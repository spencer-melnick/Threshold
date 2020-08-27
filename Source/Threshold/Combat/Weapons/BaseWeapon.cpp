// Copyright (c) 2020 Spencer Melnick

#include "BaseWeapon.h"
#include "Engine/World.h"
#include "Components/MeshComponent.h"




// Component name constants

const FName ABaseWeapon::MeshComponentName(TEXT("MeshComponent"));



// Default constructor

ABaseWeapon::ABaseWeapon()
{
	// Create a static mesh by default, but allow this to be changed in derived classes
	MeshComponent = Cast<UMeshComponent>(CreateDefaultSubobject<UStaticMeshComponent>(MeshComponentName));

	// Our tick should be disabled on start, but we want to re-enable it when we do weapon traces
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
}




// Engine overrides

void ABaseWeapon::BeginPlay()
{
	// Create some empty socket positions
	LastSocketPositions.InsertDefaulted(TraceSocketNames.Num());
}

void ABaseWeapon::Tick(float DeltaSeconds)
{
	// Check that our socket names and positions match up
	check(TraceSocketNames.Num() == LastSocketPositions.Num());

	for (int32 i = 0; i < TraceSocketNames.Num(); i++)
	{
		if (!MeshComponent->DoesSocketExist(TraceSocketNames[i]))
		{
			// A simple check to see if the socket exists before we try to update or trace
			continue;
		}
		
		const FVector NewSocketLocation = MeshComponent->GetSocketLocation(TraceSocketNames[i]);

		if (bAreSocketPositionsUpToDate)
		{
			// Do trace here
			
		}

		LastSocketPositions[i] = NewSocketLocation;
	}

	// Our socket positions will always be up to date after this!
	bAreSocketPositionsUpToDate = true;
}




// Weapon controls

void ABaseWeapon::StartWeaponTrace()
{
	// Enable tick
	SetActorTickEnabled(true);
}

void ABaseWeapon::StopWeaponTrace()
{
	// Disable tick
	SetActorTickEnabled(false);

	// Track that our weapon positions are out of date so we can update them on our next trace
	bAreSocketPositionsUpToDate = false;
}




