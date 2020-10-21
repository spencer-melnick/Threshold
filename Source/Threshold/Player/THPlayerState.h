// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/Components/InventoryOwner.h"
#include "THPlayerState.generated.h"


// Forward declarations

class UInventoryComponent;


/**
 * Class for custom player state. This contains an inventory component and other relevant information about the player that
 * should be replicated to all clients.
 */
UCLASS()
class ATHPlayerState : public APlayerState, public IInventoryOwner
{
	GENERATED_BODY()

public:
	ATHPlayerState();


	// Engine overrides

	virtual void BeginPlay() override;


	// Inventory owner overrides

	virtual UInventoryComponent* GetInventoryComponent() const override
	{
		return InventoryComponent;
	}
	

	// Component name constants

	static FName InventoryComponentName;


private:
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="THPlayerState", meta=(AllowPrivateAccess="true"))
	UInventoryComponent* InventoryComponent;
	
};
