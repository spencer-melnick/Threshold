// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"



// Forward declarations

class UNotificationSlot;



/**
 * A notification widget is designed to be held in a notification slot. It contains multiple functions that allow it
 * to control how it interacts with the notification slot, such as notifying its parent slot that it should display
 * the next widget in the queue, or to remove itself.
 */
UCLASS()
class UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

	friend UNotificationSlot;

	
public:

	// Notification slot controls
	
	// Removes this widget from its parent slot (if any) - use this instead of regular RemoveFromParent
	UFUNCTION(BlueprintCallable, Category=NotificationWidget)
	void RemoveFromParentSlot();

	// Tells the parent notification slot to show the next notification if any (does not hide or delete this widget though)
	UFUNCTION(BlueprintCallable, Category=NotificationWidget)
	void EndNotification();


	// Accessors

	UNotificationSlot* GetParentSlot() const { return ParentSlot; }


protected:

	// Events

	virtual void OnAddedToParentSlot();

	UFUNCTION(BlueprintImplementableEvent, Category=NotificationWidget, meta=(DisplayName=OnAddedToParentSlot))
	void BP_OnAddedToParentSlot();
	
	
private:

	// Parent widget reference
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UNotificationSlot* ParentSlot;

	
};
