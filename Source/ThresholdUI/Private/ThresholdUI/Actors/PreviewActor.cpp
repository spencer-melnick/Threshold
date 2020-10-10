// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Actors/PreviewActor.h"



// APreviewActor

// Component name constants

FName APreviewActor::OriginComponentName(TEXT("RootComponent"));



// Default constructor

APreviewActor::APreviewActor()
{
	// Create an empty scene component to serve as a center point
	OriginSceneComponent = CreateDefaultSubobject<USceneComponent>(OriginComponentName);
	RootComponent = OriginSceneComponent;
}

