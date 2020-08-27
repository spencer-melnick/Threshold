// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"



// Forward declarations

class UMeshComponent;
class UWeaponMoveset;



/**
 * This is the base class for all weapons 🤷
 */
UCLASS()
class ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();



	// Engine overrides

	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;



	// Weapon controls

	void StartWeaponTrace();
	void StopWeaponTrace();
	
	
	
	// Public components
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent* MeshComponent;



	// Component name constants

	static const FName MeshComponentName;



	// Editor properties

	// These are the names of all the sockets on our mesh that we want to use for performing the weapon overlap trace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TArray<FName> TraceSocketNames;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	UWeaponMoveset* Moveset = nullptr;


	
private:
	bool bAreSocketPositionsUpToDate = false;
	TArray<FVector> LastSocketPositions;
};
