// Copyright (c) 2020 Spencer Melnick

#include "InteractiveObject.h"


void IInteractiveObject::AttachInteractionIndicator(AActor* Indicator)
{
	Indicator->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Indicator->SetActorLocation(GetInteractLocation());
}
