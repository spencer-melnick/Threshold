// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMenu.generated.h"

/**
 * Main class for the player menu that contains the inventory screen and player status
 */
UCLASS()
class UPlayerMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerMenuWidget(const FObjectInitializer& ObjectInitializer);


	// Engine overrides

	virtual void NativeConstruct() override;
};
