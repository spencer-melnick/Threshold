// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"



// Forward declarations

class UMeshComponent;
class UWeaponMoveset;
class ABaseCharacter;
struct FWeaponMove;



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
	
	

	// Accessors

	ABaseCharacter* GetOwningCharacter() const;

	UMeshComponent* GetMeshComponent() const
	{
		return Cast<UMeshComponent>(RootComponent);
	}

	



	// Component name constants

	static const FName MeshComponentName;



	// Editor properties

	// These are the names of all the sockets on our mesh that we want to use for performing the weapon overlap trace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TArray<FName> TraceSocketNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	float TraceSphereRadius = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Weapon")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Weapon")
	FGameplayTag HitEventTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Weapon", meta=(Categories="GameplayCue"))
	FGameplayTag HitCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	UWeaponMoveset* Moveset = nullptr;


protected:
	// Helper functions

	void HandleHitResults(TArray<FHitResult>& HitResults, FVector HitVelocity);
	
	
	
	// Blueprint events

	// Called when the weapon trace starts locally
	UFUNCTION(BlueprintImplementableEvent, Category="Weapon")
	void OnStartWeaponTrace();

	// Called when the weapon trace stops locally
	UFUNCTION(BlueprintImplementableEvent, Category="Weapon")
    void OnStopWeaponTrace();


	
private:
	// Private variables
	
	bool bAreSocketPositionsUpToDate = false;
	TArray<FVector> LastSocketPositions;
	TArray<TWeakObjectPtr<ABaseCharacter>> DamagedCharacters;
};
