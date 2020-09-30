// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "InventoryItem.h"
#include "ItemPickup.generated.h"


// Forward declarations

class UMeshComponent;
class UInventoryItemHelper;


/**
 * Generic actor for picking up inventory objects
 */
UCLASS()
class AItemPickup : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	AItemPickup();


	// Engine overrides

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// Interactive object overrides

	virtual bool CanInteract(ABaseCharacter* Character) const override;
	virtual FVector GetInteractLocation() const override;
	virtual void AttachInteractionIndicator(AActor* Indicator) override;
	virtual void OnServerInteract(ABaseCharacter* Character) override;

	
	// Component names

	static FName MeshComponentName;



	// Editor properties

	UPROPERTY(Category="ItemPickup", EditAnywhere, BlueprintReadWrite)
	FInventoryItem InventoryItem;


private:
	UPROPERTY(Category=ItemPickup, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UMeshComponent* MeshComponent;
};
