// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppExpressionWithoutSideEffects

#include "ThresholdUI/SlateWidgets/SLiveRender.h"
#include "Rendering/DrawElements.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

/*
#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateCoreAccessibleWidgets.h"
#endif
*/



// Slate attributes

void SLiveRender::Construct(const FArguments& InArgs, UObject* WorldContextObject, UMaterialInterface* ParentMaterial)
{
	ColorAndOpacity = InArgs._ColorAndOpacity;
	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;
	DefaultTargetSize = InArgs._DefaultTargetSize;
	SetTextureParameterName(InArgs._TextureParameterName);
	OnVisibilityChangedHandler = InArgs._OnVisibilityChanged;
	OnSizeChangedHandler = InArgs._OnSizeChanged;

	// Create a new render target
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(WorldContextObject);

	// Assign the parent material
	MaterialBrush = MakeShareable(new FSlateMaterialBrush(FVector2D(SlateBrushDefs::DefaultImageSize, SlateBrushDefs::DefaultImageSize)));
	SetMaterial(ParentMaterial);
}

void SLiveRender::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	SetAttribute(ColorAndOpacity, InColorAndOpacity, EInvalidateWidgetReason::Paint);
}

void SLiveRender::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	SetColorAndOpacity(TAttribute<FSlateColor>(InColorAndOpacity));
}



// Garbage collection overrides

FString SLiveRender::GetReferencerName() const
{
	return TEXT("SLiveRender Widget");
}

void SLiveRender::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(RenderTarget);
	Collector.AddReferencedObject(MaterialInstance);
}



// Accessors

void SLiveRender::SetMaterial(UMaterialInterface* Material)
{
	// Compare materials by their base/parent materials
	UMaterialInterface* CurrentParentMaterial = nullptr;
	if (MaterialInstance)
	{
		CurrentParentMaterial = MaterialInstance->GetBaseMaterial();
	}

	if (CurrentParentMaterial == Material)
	{
		// Skip if the materials have the same base
		return;
	}
	
	if (!Material)
	{
		MaterialInstance = nullptr;
	}
	else
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(Material, nullptr);
	}

	UpdateMaterial();
}

void SLiveRender::SetTextureParameterName(FName InTextureParameterName)
{
	if (InTextureParameterName != TextureParameterName)
	{
		TextureParameterName = InTextureParameterName;
		UpdateMaterial();
	}
}



// SWidget overrides

int32 SLiveRender::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Calculate the rendered size of the widget
	const FSlateRect BoundingRect = AllottedGeometry.GetRenderBoundingRect();
	const FVector2D WidgetSize = BoundingRect.GetSize();
	const FIntPoint TargetSize(FMath::RoundToInt(WidgetSize.X), FMath::RoundToInt(WidgetSize.Y));

	if (RenderTarget && (RenderTarget->SizeX != TargetSize.X || RenderTarget->SizeY != TargetSize.Y))
	{
		// Resize the render target if necessary
		RenderTarget->ResizeTarget(TargetSize.X, TargetSize.Y);
		OnSizeChangedHandler.ExecuteIfBound();
	}
	

	// Use standard image rendering
	if (MaterialBrush.IsValid() && MaterialBrush->DrawAs != ESlateBrushDrawType::NoDrawType)
	{
		const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const FLinearColor FinalColorAndOpacity( InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * MaterialBrush->GetTint( InWidgetStyle ) );

		if (bFlipForRightToLeftFlowDirection && GSlateFlowDirection == EFlowDirection::RightToLeft)
		{
			const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(-1, 1)));
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, FlippedGeometry.ToPaintGeometry(), MaterialBrush.Get(), DrawEffects, FinalColorAndOpacity);
		}
		else
		{
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), MaterialBrush.Get(), DrawEffects, FinalColorAndOpacity);
		}
	}

	return LayerId;
}

void SLiveRender::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const bool bCurrentlyVisible = CalculateCurrentVisibility(AllottedGeometry);

	if (bCurrentlyVisible != bWasVisible)
	{
		// If our visibility changed, execute the delegate function
		bWasVisible = bCurrentlyVisible;
		OnVisibilityChangedHandler.ExecuteIfBound(bCurrentlyVisible);
	}
}


FVector2D SLiveRender::ComputeDesiredSize(float) const
{
	return DefaultTargetSize;
}



// Helper functions

void SLiveRender::UpdateMaterial()
{	
	if (MaterialInstance)
	{
		if (MaterialBrush.IsValid())
		{
			MaterialBrush->SetMaterial(MaterialInstance);
		}
		
		if (RenderTarget)
		{
			MaterialInstance->SetTextureParameterValue(TextureParameterName, RenderTarget);
		}
		else
		{
			// TODO: Only clear the specific named parameter
			MaterialInstance->ClearParameterValues();
		}
	}
}

bool SLiveRender::CalculateCurrentVisibility(const FGeometry& AllottedGeometry) const
{
	if (!GetVisibility().IsVisible())
	{
		return false;
	}

	TWeakPtr<SWidget> ParentWidget = GetParentWidget();
	while (ParentWidget.IsValid())
	{
		// Travel up the widget tree checking to see if any are invisible
		const TSharedPtr<SWidget> PinnedParent = ParentWidget.Pin();

		if (!PinnedParent->GetVisibility().IsVisible())
		{
			return false;
		}

		ParentWidget = PinnedParent->GetParentWidget();
	}
	
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FSlateRect BoundingRect = AllottedGeometry.GetRenderBoundingRect();
		const FSlateRect ViewportRect(FVector2D::ZeroVector, GEngine->GameViewport->Viewport->GetSizeXY());

		if (!FSlateRect::DoRectanglesIntersect(ViewportRect, BoundingRect))
		{
			// If the render bounding rectangle isn't in the viewport, then we aren't visible
			return false;
		}
	}

	// If all conditions are met (or the viewport is inaccessible) assume we're visible
	return true;
}

