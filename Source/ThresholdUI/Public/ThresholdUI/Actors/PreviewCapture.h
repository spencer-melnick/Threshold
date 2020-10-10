// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PreviewCapture.generated.h"


// Forward declarations

class USceneCaptureComponent2D;
class APreviewActor;



/**
 * Actor used to render 3D previews of other actors (such as inventory item models) to a render target
 */
UCLASS(Transient)
class THRESHOLDUI_API APreviewCapture : public AActor
{
	GENERATED_BODY()

public:
	APreviewCapture();



	// Engine overrides

	virtual void Tick(float DeltaSeconds) override;
	
	
	
	// Capture controls

	/**
	 * Render a single frame of this capture to the set render target. Does nothing if this capture actor is
	 * set to capture every frame
	 */
	UFUNCTION(BlueprintCallable, Category=PreviewCapture)
	void RenderSinglePreview();

	/**
	 * Attaches a target actor to this preview capture to render. Does nothing if the target actor is null. Detaches
	 * the currently attached actor if there is any.
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	void AttachTargetActor(APreviewActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	void DetachCurrentTargetActor();

	UFUNCTION(BlueprintPure, Category=CaptureControls)
	APreviewActor* GetTargetActor() const
	{
		return TargetActor;
	}

	/**
	 * Sets the render target for the capture component
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	void SetRenderTarget(UTextureRenderTarget2D* RenderTarget);

	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	void SetCapturingEveryFrame(const bool bCaptureEveryFrame);

	UFUNCTION(BlueprintPure, Category=CaptureControls)
	bool IsCapturingEveryFrame() const;

	/**
	 * Set the capture camera FOV
	 * @param NewFOV - Desired camera FOV in degrees
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	void SetFOV(const float NewFOV);

	/**
	 * Get the capture camera FOV
	 * @return - Camera FOV in degrees
	 */
	UFUNCTION(BlueprintCallable, Category=CaptureControls)
	float GetFOV() const;



	// Factory method

	/**
	 * Factory method that creates a new preview capture and initializes it with  the set properties
	 * @param WorldContextObject - Object used to get the current world from (can be whoever is spawning this)
	 * @param TargetActor - Actor holding a 3D model used for preview rendering
	 * @param RenderTarget - Render target that the preview image is rendered to
	 * @param TargetRotation - Starting rotation of the target actor once it is attached to the preview capture
	 * @param SpawnLocation - Location to place this preview capture actor (and its subsequent children) in the world
	 * @param CameraFOV - Desired camera FOV
	 * @param bCaptureEveryFame - Whether or not to capture to the render target every frame (use RenderSinglePreview
	 * in other cases as needed)
	 * @return New preview capture actor, or null if the creation failed
	 */
	UFUNCTION(BlueprintCallable, Category=PreviewCapture)
	static APreviewCapture* CreatePreviewCapture(UObject* WorldContextObject, APreviewActor* TargetActor,
		UTextureRenderTarget2D* RenderTarget, const FRotator TargetRotation = FRotator::ZeroRotator,
		const FVector SpawnLocation = FVector::ZeroVector, const float CameraFOV = 90.f, bool bCaptureEveryFame = false);
	
	

	// Component name constants

	static FName OriginComponentName;
	static FName CaptureComponentName;


protected:
	
	// Helper functions

	// Sets the camera position based on the zoom distance of the target actor
	void UpdateCameraPosition();

	
private:

	// Attached actor
	UPROPERTY()
	APreviewActor* TargetActor;

	

	// Components

	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneComponent* OriginSceneComponent;
	
	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneCaptureComponent2D* CaptureComponent;
};
