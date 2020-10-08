// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Actors/PreviewCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"


// APreviewCapture


// Component name constants

FName APreviewCapture::RootComponentName(TEXT("RootSceneComponent"));
FName APreviewCapture::CaptureComponentName(TEXT("CaptureComponent"));



// Default constructor

APreviewCapture::APreviewCapture()
{
	// Create root component
	OriginSceneComponent = CreateDefaultSubobject<USceneComponent>(RootComponentName);
	RootComponent = OriginSceneComponent;
	
	// Create default capture component
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(CaptureComponentName);
	CaptureComponent->SetupAttachment(RootComponent);

	// Enable rendering transparency
	CaptureComponent->bConsiderUnrenderedOpaquePixelAsFullyTranslucent = true;
	CaptureComponent->CaptureSource = SCS_SceneColorHDR;

	// Only render actors added to the "show" list
	CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	// Disable rendering every frame
	CaptureComponent->bCaptureEveryFrame = false;
}



// Render controls

void APreviewCapture::RenderActor(AActor* TargetActor, UTextureRenderTarget2D* RenderTarget)
{
	if (!TargetActor || !RenderTarget)
	{
		return;
	}

	// Position the actor so it takes up the whole screen
	const float AspectRatio = static_cast<float>(RenderTarget->SizeX) / static_cast<float>(RenderTarget->SizeY);
	PositionActor(TargetActor, AspectRatio);

	// Set the render target and capture the scene
	CaptureComponent->ShowOnlyActors.Add(TargetActor);
	CaptureComponent->TextureTarget = RenderTarget;
	CaptureComponent->CaptureScene();
}



// Helper functions

void APreviewCapture::PositionActor(AActor* TargetActor, float AspectRatio)
{
	// Attach the target actor
	TargetActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TargetActor->SetActorScale3D(FVector::OneVector);

	// Find the bounding box for the object
	FVector BoundsOrigin;
	FVector BoundsExtent;
	TargetActor->GetActorBounds(false, BoundsOrigin, BoundsExtent);

	// Calculate the size of the bounding sphere
	FMinimalViewInfo CaptureViewInfo;
	CaptureComponent->GetCameraView(0.f, CaptureViewInfo);
	CaptureViewInfo.AspectRatio = AspectRatio;
	const float BoundingScreenSize = ComputeBoundsScreenSize(BoundsOrigin, BoundsExtent.Size(), CaptureViewInfo.Location, CaptureViewInfo.CalculateProjectionMatrix());

	// Scale the actor so it takes up the whole screen
	TargetActor->SetActorScale3D(FVector::OneVector * FillAmount / BoundingScreenSize);
}
