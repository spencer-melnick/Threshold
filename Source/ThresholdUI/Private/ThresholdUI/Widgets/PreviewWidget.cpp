// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/PreviewWidget.h"
#include "ThresholdUI.h"
#include "ThresholdUI/Actors/PreviewCapture.h"



// UPreviewWidget

UPreviewWidget::UPreviewWidget() :
	Super()
{
	PreviewCaptureClass = APreviewCapture::StaticClass();
	PreviewCaptureSpawnLocation = FVector::DownVector * 1000.f;
}



// Capture controls

void UPreviewWidget::RenderSinglePreview()
{
	if (PreviewCapture)
	{
		PreviewCapture->RenderSinglePreview();
	}
}

void UPreviewWidget::SetPreviewActorClass(TSoftClassPtr<APreviewActor> NewPreviewClass, bool bCaptureImmediately)
{
	if (PreviewCapture)
	{
		PreviewCapture->SetPreviewActorClass(NewPreviewClass, bCaptureImmediately);
	}
}

void UPreviewWidget::ClearPreviewActorClass(bool bCapture)
{
	if (PreviewCapture)
	{
		PreviewCapture->ClearPreviewActorClass(bCapture);
	}
}



// Construction

void UPreviewWidget::BeginPlay()
{
	if (!PreviewCaptureClass)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("UPreviewWidget::NativeConstruct failed for %s - invalid preview capture class"),
			*GetNameSafe(this))
		return;
	}

	// Spawn the capture actor
	PreviewCapture = GetWorld()->SpawnActor<APreviewCapture>(PreviewCaptureClass, PreviewCaptureSpawnLocation, FRotator::ZeroRotator);
	AssignRenderTarget();
}



// Widget overrides

void UPreviewWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	
	if (!IsDesignTime())
	{
		UWorld* World = GetWorld();

		if (World && World->IsGameWorld())
		{
			BeginPlay();
		}
	}
}




// LiveRender overrides

void UPreviewWidget::OnSizeChanged()
{
	Super::OnSizeChanged();

	// Reassign our render target (and redraw if we need to)
	AssignRenderTarget();
}





// Helper functions

void UPreviewWidget::AssignRenderTarget()
{
	UTextureRenderTarget2D* RenderTarget = GetRenderTarget();
	
	if (PreviewCapture && RenderTarget)
	{
		PreviewCapture->GetCaptureComponent()->TextureTarget = RenderTarget;
		PreviewCapture->RenderSinglePreview();
	}
}
