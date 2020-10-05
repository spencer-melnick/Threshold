// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"


// Forward declarations

class UPlayerMenuWidget;



/**
 * Main HUD class for players, includes menus and screen overlays
 */
UCLASS()
class APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();


	// Engine overrides

	virtual void BeginPlay() override;



	// Editor properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPlayerMenuWidget> PlayerMenuClass;


protected:

	// Helper functions
	
	void CreateWidgets();
	


private:
	// Widgets

	UPROPERTY()
	UPlayerMenuWidget* PlayerMenuWidget;
};
