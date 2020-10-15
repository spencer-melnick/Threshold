// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Actors/PreviewCapture.h"
#include "ThresholdUI.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/AssetManager.h"
#include "Inventory/Actors/PreviewActor.h"



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
	CaptureComponent->CaptureSource = SCS_FinalColorHDR;
	CaptureComponent->ShowFlags.TemporalAA = true;
	CaptureComponent->bAlwaysPersistRenderingState = true;

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
}



// Capture controls

void APreviewCapture::RenderSinglePreview()
{
	if (CaptureComponent->bCaptureEveryFrame)
	{
		return;
	}
	
	CaptureComponent->CaptureScene();
}

void APreviewCapture::SetPreviewActorClass(TSoftClassPtr<APreviewActor> NewPreviewClass, bool bCaptureImmediately)
{
	if (PreviewActorClass.IsNull() && NewPreviewClass.IsNull() || PreviewActorClass == NewPreviewClass)
	{
		// Skip if the preview class isn't going to change
		return;
	}

	// Dump our existing preview actor and class
	DetachPreviewActor();
	PreviewActorClass = NewPreviewClass;
	StopLoading();
	
	if (PreviewActorClass.IsNull())
	{
		// If the new class is invalid, skip loading
		return;
	}

	// Request that our preview actor be loaded asynchronously
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	PreviewActorStreamableHandle = StreamableManager.RequestAsyncLoad(NewPreviewClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &APreviewCapture::OnAssetLoaded, bCaptureImmediately));
}

void APreviewCapture::ClearPreviewActorClass(bool bCapture)
{
	StopLoading();
	DetachPreviewActor();

	if (bCapture)
	{
		RenderSinglePreview();
	}
}





// Helper functions

void APreviewCapture::AttachPreviewActor(APreviewActor* NewActor)
{
	if (!NewActor)
	{
		// Skip if the new preview actor is null
		return;
	}
	
	if (NewActor == PreviewActor)
	{
		// Skip if nothing has changed
		return;
	}

	// Detach the current preview actor
	DetachPreviewActor();

	// Assign the new preview actor and attach it
	PreviewActor = NewActor;
	PreviewActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CaptureComponent->SetRelativeLocation(FVector::BackwardVector * PreviewActor->CameraDistance);

	// Add our preview actor to the capture list
	CaptureComponent->ShowOnlyActors.Add(PreviewActor);
}

void APreviewCapture::DetachPreviewActor()
{
	if (!PreviewActor)
	{
		// Skip if there is no preview actor currently
		return;
	}
	
	// Reset the capture list
	CaptureComponent->ShowOnlyActors.Reset(1);

	// Physically detach the preview actor and destroy it
	PreviewActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	PreviewActor->Destroy();

	// Clear reference to preview actor
	PreviewActor = nullptr;
}

void APreviewCapture::StopLoading()
{
	if (PreviewActorStreamableHandle.IsValid() && PreviewActorStreamableHandle->IsLoadingInProgress())
	{
		// If we're loading something already, cancel it
		PreviewActorStreamableHandle->CancelHandle();
	}
}




// Delegates

void APreviewCapture::OnAssetLoaded(bool bCaptureImmediately)
{
	if (!PreviewActorStreamableHandle.IsValid())
	{
		// Handle could be invalidated if the asset is dumped elsewhere while we're still loading
		UE_LOG(LogThresholdUI, Error, TEXT("APreviewCapture::OnAssetLoaded failed for %s - asset handle is invalid"),
			*GetNameSafe(this))
		return;
	}

	if (!PreviewActorStreamableHandle->HasLoadCompleted())
	{
		// For some reason the delegate has been called but the load isn't completed
		UE_LOG(LogThresholdUI, Error, TEXT("APreviewCapture::OnAssetLoaded failed for %s - asset handle hasn't finished loading"),
            *GetNameSafe(this))
		return;
	}

	UClass* LoadedActorClass = Cast<UClass>(PreviewActorStreamableHandle->GetLoadedAsset());

	if (!LoadedActorClass || !LoadedActorClass->IsChildOf<APreviewActor>())
	{
		UE_LOG(LogThresholdUI, Error, TEXT("APreviewCapture::OnAssetLoaded failed for %s - failed to load a valid APreviewActor class"),
            *GetNameSafe(this))
		return;
	}

	// Spawn our new actor
	APreviewActor* NewPreviewActor = Cast<APreviewActor>(GetWorld()->SpawnActor(LoadedActorClass));
	AttachPreviewActor(NewPreviewActor);

	// Render a capture if needed
	if (bCaptureImmediately)
	{
		RenderSinglePreview();
	}
}

