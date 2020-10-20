﻿// Copyright (c) 2020 Spencer Melnick

#include "InteractionSubsystem.h"
#include "Threshold/World/InteractiveObject.h"



// UInteractionSubsystem

void UInteractionSubsystem::RegisterObject(IInteractiveObject* Object)
{
	InteractiveObjects.AddUnique(TWeakInterfacePtr<IInteractiveObject>(*Object));
}

void UInteractionSubsystem::UnregisterObject(IInteractiveObject* Object)
{
	InteractiveObjects.RemoveAll([Object](const TWeakInterfacePtr<IInteractiveObject>& OtherObject) -> bool
	{
		// Also remove all invalid objects while we're removing
		return Object == &(*OtherObject) || !OtherObject.IsValid();
	});
}
