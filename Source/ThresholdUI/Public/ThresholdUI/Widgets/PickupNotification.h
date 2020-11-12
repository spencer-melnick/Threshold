// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdUI/Widgets/NotificationWidget.h"
#include "PickupNotification.generated.h"



// Forward declarations

struct FInventoryItem;
class UInventoryBlock;
class URichTextBlock;



/**
 * Widget that shows a notification when the player picks up an item
 */
UCLASS()
class UPickupNotification : public UNotificationWidget
{
	GENERATED_BODY()

public:
	
	// Display controls

	// Updates the text and thumbnail to show the details of a specific item
	void SetDisplayedItem(FInventoryItem& Item);

	
	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TextFormat)
	FText NotificationText;
	

protected:

	// Helper functions

	/**
	 * Updates the pre-compiled format text if it does not match the text as set in the editor property
	 */
	void UpdateFormatText();


private:

	// Child widgets

	UPROPERTY(meta=(BindWidget))
	UInventoryBlock* ItemDisplay;

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* TextDisplay;


	// Pre-compiled text format
	FTextFormat NotificationTextFormat;
	
};
