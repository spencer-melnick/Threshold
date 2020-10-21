// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.h"
#include "PlayerMenu.generated.h"

/**
 * Main class for the player menu that contains the inventory screen and player status
 */
UCLASS()
class THRESHOLDUI_API UPlayerMenuWidget : public UUserWidget, public IPlayerWidgetInterface
{
	GENERATED_BODY()
	
public:
	UPlayerMenuWidget(const FObjectInitializer& ObjectInitializer);


	// Engine overrides

	virtual void NativeConstruct() override;



	// Initialization
	
	virtual void OnPlayerStateInitialized() override;
};
