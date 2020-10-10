// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PreviewActor.generated.h"


/**
 * Class used for objects that are rendered in a preview capture for the UI. Essentially contains an empty
 * scene component to use as the origin and a single property that describes the desired camera distance
 */
UCLASS()
class THRESHOLDUI_API APreviewActor : public AActor
{
	GENERATED_BODY()

public:
	APreviewActor();


	// Component name constants

	static FName OriginComponentName;


	// Editor properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PreviewActor)
	float CameraDistance = 1000.f;
	

private:

	// Components

	UPROPERTY(Category=PreviewActor, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneComponent* OriginSceneComponent;
};
