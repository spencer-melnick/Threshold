// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/NotificationSlot.h"
#include "ThresholdUI/Widgets/NotificationWidget.h"
#include "ThresholdUI.h"
#include "Components/Overlay.h"



// UNotificationSlot

// Widget overrides

#if WITH_EDITOR
void UNotificationSlot::SynchronizeProperties()
{
	CHECK_WIDGET_STATEMENT(OverlaySlot)
	
	const UWorld* World = GetWorld();

	if (World && !World->IsGameWorld())
	{
		// Add a preview widget if this isn't the game world
		if (PreviewWidget)
		{
			OverlaySlot->RemoveChild(PreviewWidget);
		}
		if (PreviewWidgetClass)
		{
			PreviewWidget = CreateWidget<UNotificationWidget>(this, PreviewWidgetClass);
			if (PreviewWidget)
			{
				OverlaySlot->AddChildToOverlay(PreviewWidget);
			}
		}
	}
}
#endif



// Notification controls

void UNotificationSlot::AddChildNotification(UNotificationWidget* Widget)
{
	CHECK_WIDGET_STATEMENT(OverlaySlot)
	
	if (!Widget)
	{
		return;
	}
	
	QueuedNotificationWidgets.Add(Widget);
	CheckNextNotification();
}

void UNotificationSlot::EndChildNotification(UNotificationWidget* Widget)
{
	if (!Widget || Widget != BlockingNotificationWidget)
	{
		return;
	}

	BlockingNotificationWidget = nullptr;
	CheckNextNotification();
}


void UNotificationSlot::RemoveChildNotification(UNotificationWidget* Widget)
{
	CHECK_WIDGET_STATEMENT(OverlaySlot)
	
	if (!Widget)
	{
		return;
	}
	
	if (Widget == BlockingNotificationWidget)
	{
		// If the removed widget was active, end its blocking state
		EndChildNotification(Widget);
	}
	QueuedNotificationWidgets.Remove(Widget);
	if (ActiveNotificationWidgets.Remove(Widget) > 0)
	{
		OverlaySlot->RemoveChild(Widget);
	}
}

void UNotificationSlot::CheckNextNotification()
{
	CHECK_WIDGET_STATEMENT(OverlaySlot)
	
	if (!BlockingNotificationWidget && QueuedNotificationWidgets.Num() >= 1)
	{
		BlockingNotificationWidget = QueuedNotificationWidgets[0];
		QueuedNotificationWidgets.RemoveAt(0);
		ActiveNotificationWidgets.Add(BlockingNotificationWidget);
		BlockingNotificationWidget->ParentSlot = this;
		OverlaySlot->AddChildToOverlay(BlockingNotificationWidget);
		BlockingNotificationWidget->OnAddedToParentSlot();
	}
}

