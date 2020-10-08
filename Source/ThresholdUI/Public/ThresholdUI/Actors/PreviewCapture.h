// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PreviewCapture.generated.h"


// Forward declarations

class USceneCaptureComponent2D;



/**
 * Actor used to render 3D previews of other actors (such as inventory item models) to a render target
 */
UCLASS(Blueprintable)
class THRESHOLDUI_API APreviewCapture : public AActor
{
	GENERATED_BODY()

public:
	APreviewCapture();



	// Render controls

	/**
	 * Render an actor to the specified render target, attempting to fit the screen size.
	 * Actors rendered with this function will become owned by the preview capture actor and should no longer be visible
	 * in the scene. The render will only utilize lighting from the third lighting channel.
	 * @param TargetActor - Actor to be rendered
	 * @param RenderTarget - Render target where the preview will be rendered to
	 */
	UFUNCTION(BlueprintCallable, Category=PreviewCapture)
	void RenderActor(AActor* TargetActor, UTextureRenderTarget2D* RenderTarget);

	

	// Component name constants

	static FName RootComponentName;
	static FName CaptureComponentName;



	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PreviewCatpure)
	float FillAmount = 0.5f;


protected:

	// Helper functions

	void PositionActor(AActor* TargetActor, float AspectRatio);


private:

	// Components

	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	USceneComponent* OriginSceneComponent;
	
	UPROPERTY(Category=PreviewCapture, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	USceneCaptureComponent2D* CaptureComponent;
};
