// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppExpressionWithoutSideEffects
#include "ThresholdUI/Widgets/LiveRender.h"
#include "Engine/TextureRenderTarget2D.h"



// ULiveRender

// Accessors

void ULiveRender::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;

	if (LiveRenderWidget.IsValid())
	{
		LiveRenderWidget->SetColorAndOpacity(ColorAndOpacity);
	}
}

void ULiveRender::SetOpacity(float InOpacity)
{
	ColorAndOpacity.A = InOpacity;

	if (LiveRenderWidget.IsValid())
	{
		LiveRenderWidget->SetColorAndOpacity(ColorAndOpacity);
	}
}

void ULiveRender::SetDefaultTargetSize(FIntPoint InDefaultTargetSize)
{
	DefaultTargetSize = InDefaultTargetSize;

	if (LiveRenderWidget.IsValid())
	{
		LiveRenderWidget->SetDefaultTargetSize(InDefaultTargetSize);
	}
}

void ULiveRender::SetTextureParameterName(FName InTextureParameterName)
{
	TextureParameterName = InTextureParameterName;

	if (LiveRenderWidget.IsValid())
	{
		LiveRenderWidget->SetTextureParameterName(InTextureParameterName);
	}
}

void ULiveRender::SetParentMaterial(UMaterialInterface* InParentMaterial)
{
	ParentMaterial = InParentMaterial;

	if (LiveRenderWidget.IsValid())
	{
		LiveRenderWidget->SetMaterial(InParentMaterial);
	}
}

UMaterialInstanceDynamic* ULiveRender::GetMaterialInstance() const
{
	if (!LiveRenderWidget.IsValid())
	{
		return nullptr;
	}

	return LiveRenderWidget->GetMaterialInstance();
}

UTextureRenderTarget2D* ULiveRender::GetRenderTarget() const
{
	if (!LiveRenderWidget.IsValid())
	{
		return nullptr;
	}

	return LiveRenderWidget->GetRenderTarget();
}



// Widget overrides

void ULiveRender::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!LiveRenderWidget.IsValid())
	{
		return;
	}

	LiveRenderWidget->SetColorAndOpacity(ColorAndOpacity);
	LiveRenderWidget->SetDefaultTargetSize(DefaultTargetSize);
	LiveRenderWidget->SetTextureParameterName(TextureParameterName);
	LiveRenderWidget->SetMaterial(ParentMaterial);
}

void ULiveRender::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	LiveRenderWidget.Reset();
}

TSharedRef<SWidget> ULiveRender::RebuildWidget()
{
	LiveRenderWidget = SNew(SLiveRender, this, ParentMaterial)
		.ColorAndOpacity(ColorAndOpacity)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection)
		.DefaultTargetSize(DefaultTargetSize)
		.TextureParameterName(TextureParameterName)
		.OnVisibilityChanged(BIND_UOBJECT_DELEGATE(FVisibilityChangedHandler, OnVisibilityChanged))
		.OnSizeChanged(BIND_UOBJECT_DELEGATE(FSizeChangedHandler, OnSizeChanged));

	return LiveRenderWidget.ToSharedRef();
}



// Delegates

void ULiveRender::OnVisibilityChanged(bool bIsVisible)
{
	bVisible = bIsVisible;
	
	OnVisibilityChangedEvent.ExecuteIfBound(bIsVisible);
}

void ULiveRender::OnSizeChanged()
{
	if (LiveRenderWidget.IsValid())
	{
		const UTextureRenderTarget2D* RenderTarget = LiveRenderWidget->GetRenderTarget();

		if (RenderTarget)
		{
			RenderTargetSize.X = RenderTarget->SizeX;
			RenderTargetSize.Y = RenderTarget->SizeY;
		}
	}


	OnSizeChangedEvent.ExecuteIfBound();
}

