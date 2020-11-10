// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/NotificationWidget.h"
#include "ThresholdUI/Widgets/NotificationSlot.h"
#include "ThresholdUI.h"



// UNotificationWidget

// Notification slot controls

void UNotificationWidget::RemoveFromParentSlot()
{
	if (!ParentSlot)
	{
		return;
	}

	ParentSlot->RemoveChildNotification(this);
}

void UNotificationWidget::EndNotification()
{
	if (!ParentSlot)
	{
		return;
	}

	ParentSlot->EndChildNotification(this);
}



// Events

void UNotificationWidget::OnAddedToParentSlot()
{
	BP_OnAddedToParentSlot();
}

