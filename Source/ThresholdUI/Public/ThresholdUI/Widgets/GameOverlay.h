// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay.generated.h"



// Forward declarations

struct FInventoryItem;
class UPickupNotification;



/**
 * Central widget for displaying information overlaid on the 3D viewport, such as player health, notifications,
 * dialogue boxes, etc.
 */
UCLASS()
class UGameOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	// Overlay controls

	void ShowItemPickupNotification(FInventoryItem& Item);



private:

	// Child widgets

	UPROPERTY(meta=(BindWidget));
	UPickupNotification* PickupDisplay;
	
};
