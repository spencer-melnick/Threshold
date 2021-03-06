﻿// Copyright (c) 2020 Spencer Melnick

#include "ThresholdGame/Combat/Weapons/BaseWeapon.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/World.h"
#include "ThresholdGame/Character/BaseCharacter.h"
#include "ThresholdGame/Combat/Teams.h"
#include "ThresholdGame/Abilities/TargetDataTypes.h"
#include "ThresholdGame/Abilities/THAbilitySystemComponent.h"
#include "ThresholdGame/Abilities/AbilityFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/MeshComponent.h"


// Component name constants

const FName ABaseWeapon::MeshComponentName(TEXT("MeshComponent"));



// Default constructor

ABaseWeapon::ABaseWeapon()
{
	// Create a static mesh by default, but allow this to be changed in derived classes
	RootComponent = Cast<UMeshComponent>(CreateDefaultSubobject<UStaticMeshComponent>(MeshComponentName));

	// Our tick should be disabled on start, but we want to re-enable it when we do weapon traces
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;

	// Replicate weapon actor spawning
	bReplicates = true;
}




// Engine overrides

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// Create some empty socket positions
	LastSocketPositions.InsertZeroed(0, TraceSocketNames.Num());
}

void ABaseWeapon::Tick(float DeltaSeconds)
{
	// Check that our socket names and positions match up
	check(TraceSocketNames.Num() == LastSocketPositions.Num());

	// Create our trace shape from the editor property
	const FCollisionShape TraceShape = FCollisionShape::MakeSphere(TraceSphereRadius);

	for (int32 i = 0; i < TraceSocketNames.Num(); i++)
	{
		if (!GetMeshComponent()->DoesSocketExist(TraceSocketNames[i]))
		{
			// A simple check to see if the socket exists before we try to update or trace
			continue;
		}
		
		const FVector NewSocketLocation = GetMeshComponent()->GetSocketLocation(TraceSocketNames[i]);
		const FVector OldSocketLocation = LastSocketPositions[i];

		if (bAreSocketPositionsUpToDate)
		{
			// Do trace here
			TArray<FHitResult> HitResults;
			if (GetWorld()->SweepMultiByChannel(HitResults, OldSocketLocation, NewSocketLocation, FQuat::Identity,
				TraceChannel,  TraceShape))
			{
				const FVector EstimatedHitVelocity = (NewSocketLocation - OldSocketLocation) / DeltaSeconds;
				HandleHitResults(HitResults, EstimatedHitVelocity);
			}
		}

		LastSocketPositions[i] = NewSocketLocation;
	}

	// Our socket positions will always be up to date after this!
	bAreSocketPositionsUpToDate = true;
}




// Accessors

ABaseCharacter* ABaseWeapon::GetOwningCharacter() const
{
	if (!RootComponent)
	{
		return nullptr;
	}

	AActor* OwningActor = RootComponent->GetAttachmentRootActor();

	if (OwningActor == this || OwningActor == nullptr)
	{
		// If this actor isn't attached to anything, the attachment root will be this
		return nullptr;
	}

	return Cast<ABaseCharacter>(OwningActor);
}




// Weapon controls

void ABaseWeapon::StartWeaponTrace()
{
	// Enable tick
	SetActorTickEnabled(true);

	// Call our optional Blueprint logic
	OnStartWeaponTrace();
}

void ABaseWeapon::StopWeaponTrace()
{
	// Disable tick
	SetActorTickEnabled(false);

	// Track that our weapon positions are out of date so we can update them on our next trace
	bAreSocketPositionsUpToDate = false;

	// Clear our damaged actors
	DamagedCharacters.Empty();

	// Call our optional Blueprint logic
	OnStopWeaponTrace();
}




// Helper functions

void ABaseWeapon::HandleHitResults(TArray<FHitResult>& HitResults, FVector HitVelocity)
{
	ABaseCharacter* OwningCharacter = GetOwningCharacter();
	UTHAbilitySystemComponent* AbilitySystemComponent = OwningCharacter->GetTHAbilitySystemComponent();

	if (!OwningCharacter || !AbilitySystemComponent)
	{
		return;
	}

	for (FHitResult& HitResult : HitResults)
	{
		// Note that with this cast we don't support all combatants - only ones derived from BaseCharacter
		ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.Actor);

		if (!HitCharacter || !HitCharacter->GetCanBeDamagedBy(OwningCharacter->GetTeam()) ||
			DamagedCharacters.Contains(HitCharacter))
		{
			continue;
		}

		// Track the damaged actors to prevent firing too many times
		DamagedCharacters.Add(HitCharacter);

		// Send a gameplay event to our source actor
		FGameplayEventData SourceEventData = UAbilityFunctionLibrary::CreateGameplayEvent(OwningCharacter, HitCharacter, HitSourceEventTag);
		SourceEventData.TargetData.Add(new FWeaponHitTargetData(HitResult, HitVelocity));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningCharacter, HitSourceEventTag, SourceEventData);

		// Send a gameplay event to our target actor
		FGameplayEventData TargetEventData = UAbilityFunctionLibrary::CreateGameplayEvent(OwningCharacter, HitCharacter, HitTargetEventTag);
		TargetEventData.TargetData.Add(new FWeaponHitTargetData(HitResult, HitVelocity));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningCharacter, HitTargetEventTag, TargetEventData);

		// Create a new gameplay cue from our data
		FGameplayCueParameters CueParameters = UAbilityFunctionLibrary::CreateGameplayCue(OwningCharacter, this, HitCharacter);
		CueParameters.EffectContext = FGameplayEffectContextHandle(new FGameplayEffectContext());
		CueParameters.EffectContext.AddHitResult(HitResult);
		CueParameters.Location = HitResult.Location;
		CueParameters.Normal = -HitVelocity.GetSafeNormal();

		// Dispatch a local gameplay cue
		AbilitySystemComponent->ExecuteGameplayCueLocal(HitCueTag, CueParameters);
	}
}

