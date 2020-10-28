// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PreviewCapture.generated.h"


// Forward declarations

class USceneCaptureComponent2D;
class APreviewActor;
struct FStreamableHandle;



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



	// Accessors
	
	UFUNCTION(BlueprintPure, Category=CaptureControls)
	APreviewActor* GetPreviewActor() const
	{
		return PreviewActor;
	}

	USceneCaptureComponent2D* GetCaptureComponent() const
	{
		return CaptureComponent;
	}


	
	// Component name constants

	static FName OriginComponentName;
	static FName CaptureComponentName;



	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CaptureControls)
	bool bRotateActor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CaptureControls)
	FRotator RotationSpeed;


protected:
	
	// Helper functions

	/**
	 * Attaches the preview actor and adds it to the visibility list
	 * @param NewActor - Preview actor to be attached. Skips execution if null
	 */
	void AttachPreviewActor(APreviewActor* NewActor);

	/**
	 * Detaches and destroys the existing preview actor (if any)
	 */
	void DetachPreviewActor();

	/**
	 * Stops loading the current preview actor (if any)
	 */
	void StopLoading();



	// Delegates

	void OnAssetLoaded(bool bCaptureImmediately);
	

	
private:

	// Desired actor class
	UPROPERTY()
	TSoftClassPtr<APreviewActor> PreviewActorClass;
	
	// Attached actor
	UPROPERTY()
	APreviewActor* PreviewActor;

	// Used for streaming the preview actor
	TSharedPtr<FStreamableHandle> PreviewActorStreamableHandle;

	

	// Components

	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneComponent* OriginSceneComponent;
	
	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	USceneCaptureComponent2D* CaptureComponent;
};
