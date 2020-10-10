// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Actors/PreviewCapture.h"
#include "ThresholdUI.h"
#include "ThresholdUI/Actors/PreviewActor.h"
#include "Components/SceneCaptureComponent2D.h"


// APreviewCapture


// Component name constants

FName APreviewCapture::OriginComponentName(TEXT("RootSceneComponent"));
FName APreviewCapture::CaptureComponentName(TEXT("CaptureComponent"));



// Default constructor

APreviewCapture::APreviewCapture()
{
	// Create root component
	OriginSceneComponent = CreateDefaultSubobject<USceneComponent>(OriginComponentName);
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

	// Start with tick disabled but allow it to be enabled later as needed
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}



// Engine overrides

void APreviewCapture::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCameraPosition();
}



// Capture controls

void APreviewCapture::RenderSinglePreview()
{
	if (IsCapturingEveryFrame())
	{
		return;
	}
	
	UpdateCameraPosition();
	CaptureComponent->CaptureScene();
}

void APreviewCapture::AttachTargetActor(APreviewActor* NewTargetActor)
{
	if (!NewTargetActor)
	{
		return;
	}

	if (NewTargetActor != TargetActor)
	{
		// Detach the current target actor if there is one
		DetachCurrentTargetActor();
	}

	// Assign the new target actor and attach it
	TargetActor = NewTargetActor;
	NewTargetActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// Add our target actor to the capture list
	CaptureComponent->ShowOnlyActors.Reset(1);
	CaptureComponent->ShowOnlyActors.Add(TargetActor);
}

void APreviewCapture::DetachCurrentTargetActor()
{
	// Reset the capture list
	CaptureComponent->ShowOnlyActors.Reset(1);

	// Physically detach the target actor
	TargetActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Clear reference to target actor
	TargetActor = nullptr;
}

void APreviewCapture::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	CaptureComponent->TextureTarget = RenderTarget;
}

void APreviewCapture::SetCapturingEveryFrame(const bool bCaptureEveryFrame)
{
	// Assign the new value and enable ticking if necessary
	CaptureComponent->bCaptureEveryFrame = bCaptureEveryFrame;
	SetActorTickEnabled(bCaptureEveryFrame);
}

bool APreviewCapture::IsCapturingEveryFrame() const
{
	return CaptureComponent->bCaptureEveryFrame;
}

void APreviewCapture::SetFOV(const float NewFOV)
{
	CaptureComponent->FOVAngle = NewFOV;
}

float APreviewCapture::GetFOV() const
{
	return CaptureComponent->FOVAngle;
}



// Factory method

APreviewCapture* APreviewCapture::CreatePreviewCapture(UObject* WorldContextObject, APreviewActor* TargetActor,
	UTextureRenderTarget2D* RenderTarget, const FRotator TargetRotation, const FVector SpawnLocation,
	const float CameraFOV, bool bCaptureEveryFame)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		UE_LOG(LogThresholdUI, Error, TEXT("Cannot create new preview capture actor - invalid world context object provided"))
		return nullptr;
	}

	// Spawn the new preview actor
	UWorld* World = WorldContextObject->GetWorld();
	APreviewCapture* NewCaptureActor = World->SpawnActor<APreviewCapture>(StaticClass(), SpawnLocation, FRotator::ZeroRotator);

	// Set the default parameters
	NewCaptureActor->AttachTargetActor(TargetActor);
	NewCaptureActor->SetRenderTarget(RenderTarget);
	NewCaptureActor->SetFOV(CameraFOV);
	NewCaptureActor->SetCapturingEveryFrame(bCaptureEveryFame);

	// Set the target actor's default rotation (if the target actor exists)
	if (TargetActor)
	{
		TargetActor->SetActorRotation(TargetRotation, ETeleportType::ResetPhysics);
	}

	return NewCaptureActor;
}



// Helper functions

void APreviewCapture::UpdateCameraPosition()
{
	if (!TargetActor)
	{
		return;
	}
	
	CaptureComponent->SetRelativeLocation(FVector::BackwardVector * TargetActor->CameraDistance);
}

