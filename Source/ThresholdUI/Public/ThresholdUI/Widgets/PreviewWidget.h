// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdUI/Widgets/LiveRender.h"
#include "PreviewWidget.generated.h"


// Forward declarations

class APreviewCapture;
class APreviewActor;



/**
 * Widget that displays a preview of a 3D model
 */
UCLASS()
class UPreviewWidget : public ULiveRender
{
	GENERATED_BODY()

public:

	UPreviewWidget();

	
	// Accessors

	APreviewCapture* GetPreviewCapture() const
	{
		return PreviewCapture;
	}


	// Capture controls

	/**
	 * Render a single frame of this capture to the set render target. Does nothing if this capture actor is
	 * set to capture every frame
	 */
	UFUNCTION(BlueprintCallable, Category=PreviewCapture)
    void RenderSinglePreview();

	/**
	 * Sets the preview actor by class and spawns the actor as needed - will load asynchronously
	 * @param NewPreviewClass - Class for the preview actor. Skips execution if null
	 * @param bCaptureImmediately - If true, a capture will be rendered as soon as the actor is loaded
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
    void SetPreviewActorClass(TSoftClassPtr<APreviewActor> NewPreviewClass, bool bCaptureImmediately = true);

	/**
	 * Clears the preview actor class, destroying and detaching the preview actor as needed. Any load operations will
	 * be canceled as well
	 * @param bCapture - If true, a capture will be rendered (useful for setting the preview to a default/clear state)
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
    void ClearPreviewActorClass(bool bCapture = true);
	

	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CaptureControls)
	TSubclassOf<APreviewCapture> PreviewCaptureClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CaptureControls)
	FVector PreviewCaptureSpawnLocation;
	

protected:

	// Construction

	virtual void BeginPlay();
	

	// Widget overrides

	virtual void OnWidgetRebuilt() override;
	

	// LiveRender overrides

	virtual void OnSizeChanged() override;


	// Helper functions

	/**
	 * Assigns the render target owned by this (as a ULiveRender) to the target of the preview capture actor and
	 * renders a new capture if necessary
	 */
	void AssignRenderTarget();


private:

	UPROPERTY()
	APreviewCapture* PreviewCapture;
};
