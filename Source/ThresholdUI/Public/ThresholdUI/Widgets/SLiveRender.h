// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Widgets/SLeafWidget.h"
#include "SlateMaterialBrush.h"


// Delegates
DECLARE_DELEGATE_OneParam(FVisibilityChangedHandler, bool)
DECLARE_DELEGATE(FSizeChangedHandler)


/**
 * Slate widget that maintains a render target with the correct size
 */
class THRESHOLDUI_API SLiveRender : public SLeafWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS( SLiveRender )
		: _ColorAndOpacity( FLinearColor::White )
	    , _FlipForRightToLeftFlowDirection( false )
		, _DefaultTargetSize( FIntPoint(128, 128) )
		, _TextureParameterName( NAME_None )
		, _OnVisibilityChanged( nullptr )
		, _OnSizeChanged( nullptr )
		{ }

		/** Color and opacity */
		SLATE_ATTRIBUTE( FSlateColor, ColorAndOpacity )

	    /** Flips the image if the localization's flow direction is RightToLeft */
	    SLATE_ARGUMENT( bool, FlipForRightToLeftFlowDirection )

		/** Default size if widget does not fill parent */
		SLATE_ARGUMENT( FIntPoint, DefaultTargetSize )

		/** Material parameter name where render target is to be assigned */
		SLATE_ARGUMENT( FName, TextureParameterName )

		/** Invoked when the widget becomes physically visible or invisible on the screen */
		SLATE_EVENT( FVisibilityChangedHandler, OnVisibilityChanged )

		/** Invoked when the widget's render target is resized */
		SLATE_EVENT( FSizeChangedHandler, OnSizeChanged )
	SLATE_END_ARGS()


	/** Constructor */
	SLiveRender()
	{
		SetCanTick(true);
		bCanSupportFocus = false;
	}

	void Construct( const FArguments& InArgs, UObject* WorldContextObject, UMaterialInterface* ParentMaterial );

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity( const TAttribute<FSlateColor>& InColorAndOpacity );
	
	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity( FLinearColor InColorAndOpacity );



	// Garbage collection overrides

	virtual FString GetReferencerName() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;



	// Accessors

	void SetDefaultTargetSize( FIntPoint InDefaultTargetSize ) { DefaultTargetSize = InDefaultTargetSize; }
	FIntPoint GetDefaultTargetSize() const { return DefaultTargetSize; }

	void SetMaterial(UMaterialInterface* Material);
	UMaterialInstanceDynamic* GetMaterialInstance() const { return MaterialInstance; }
	
	void SetTextureParameterName(FName InTextureParameterName);
	FName GetTextureParameterName() const { return TextureParameterName; }

	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }
	
	
	
	// SWidget overrides
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

/** TODO: figure out how to set up the correct accessible widget
#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget, ESPMode::Fast> CreateAccessibleWidget() override;
#endif
*/

protected:
	virtual FVector2D ComputeDesiredSize(float) const override;



	// Helper functions

	void UpdateMaterial();

	/**
	 * Calculates if the widget is rendered by traversing widget tree and using rendering bounding box
	 */
	bool CalculateCurrentVisibility(const FGeometry& AllottedGeometry) const;

	
	
	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Flips the image if the localization's flow direction is RightToLeft */
	bool bFlipForRightToLeftFlowDirection;

	/** Default size if widget does not fill parent */
	FIntPoint DefaultTargetSize;

	/** Material parameter name where render target is to be assigned */
	FName TextureParameterName;

	FVisibilityChangedHandler OnVisibilityChangedHandler;
	FSizeChangedHandler OnSizeChangedHandler;



	// Visibility tracking
	
	bool bWasVisible = false;

	

	// Render resources
	
	UMaterialInstanceDynamic* MaterialInstance = nullptr;
	UTextureRenderTarget2D* RenderTarget = nullptr;
	TSharedPtr<FSlateMaterialBrush> MaterialBrush;
};
