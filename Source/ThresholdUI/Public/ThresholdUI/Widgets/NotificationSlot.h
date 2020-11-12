// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationSlot.generated.h"



// Forward declarations

class UOverlay;
class UOverlaySlot;
class UNotificationWidget;



/**
 * Notification slots are widgets used to display a series of notifications/dialogue boxes. Their main advantage
 * is that they can control how to handle multiple dialogue boxes appearing at the same time, usually by storing
 * the notification widgets in a queue and displaying them in order
 */
UCLASS()
class UNotificationSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	// Widget overrides

	#if WITH_EDITOR
		virtual void SynchronizeProperties() override;
	#endif
	

	// Notification controls

	// Adds the widget to the queue, and displays it if its the first in the queue and the slot is enabled
	UFUNCTION(BlueprintCallable, Category=NotificationSlot)
	void AddChildNotification(UNotificationWidget* Widget);

	// Stops the widget from blocking future notifications if it is currently active, showing the next notification if possible
	UFUNCTION(BlueprintCallable, Category=NotificationSlot)
	void EndChildNotification(UNotificationWidget* Widget);

	// Remove the widget from the screen and queue if possible
	UFUNCTION(BlueprintCallable, Category=NotificationSlot)
	void RemoveChildNotification(UNotificationWidget* Widget);


	// Editor properties

	#if WITH_EDITOR
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=NotificationSlot)
		TSubclassOf<UNotificationWidget> PreviewWidgetClass;
	#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=NotificationSlot)
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=NotificationSlot)
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill;


protected:
	
	// Shows the next notification if there is one in the queue
	void CheckNextNotification();
	

private:
	
	// Bound widgets

	UPROPERTY(meta=(BindWidget))
	UOverlay* OverlayWidget;

	#if WITH_EDITOR
		UPROPERTY()
		UNotificationWidget* PreviewWidget;
	#endif


	// Notification tracking

	// Queued notification widgets that haven't been displayed yet
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<UNotificationWidget*> QueuedNotificationWidgets;

	// Notification widget currently blocking additional notifications
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UNotificationWidget* BlockingNotificationWidget;

	// Notification widgets that are currently displayed
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<UNotificationWidget*> ActiveNotificationWidgets;

};
