// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/HUD/PlayerHUD.h"
#include "ThresholdUI/Widgets/PlayerMenu.h"
#include "ThresholdUI/Widgets/GameOverlay.h"
#include "ThresholdUI.h"
#include "Blueprint/UserWidget.h"


// APlayerHUD

APlayerHUD::APlayerHUD()
{
	
}



// Engine overrides

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
	
	CreateWidgets();

	// Set default status this way to hide appropriate widgets
	SetStatus(EPlayerHUDStatus::WorldView);

	if (PlayerOwner && PlayerOwner->PlayerState)
	{
		// If the player state was created before we were, we can go ahead and run player state initialization now
		OnPlayerStateInitialized();
	}
}



// Accessors

void APlayerHUD::SetStatus(const EPlayerHUDStatus NewStatus)
{
	if (!PlayerOwner)
	{
		return;
	}
	
	Status = NewStatus;

	switch (Status)
	{
		case EPlayerHUDStatus::WorldView:
			PlayerMenuWidget->DisableWidget();
			break;

		case EPlayerHUDStatus::PlayerMenuActive:
			PlayerMenuWidget->EnableWidget();
			break;
	}
}

bool APlayerHUD::ShouldEnableCharacterControl() const
{
	switch (Status)
	{
		case EPlayerHUDStatus::WorldView:
			return true;
		default:
			return false;
	}
}

void APlayerHUD::ShowItemPickupNotification(FInventoryItem& Item)
{
	if (!GameOverlay)
	{
		return;
	}

	GameOverlay->ShowItemPickupNotification(Item);
}



// Initialization

void APlayerHUD::OnPlayerStateInitialized()
{
	// If the player state exists, the player controller should as well
	check(PlayerOwner);

	// By enabling input temporarily, we create an input component
	EnableInput(PlayerOwner);
	
	if (PlayerMenuWidget)
	{
		// Notify our widget that the player state exists
		PlayerMenuWidget->OnPlayerStateInitialized();
	}

	// Disable input for now
	DisableInput(PlayerOwner);
}




// Helper functions

void APlayerHUD::CreateWidgets()
{
	CreateWidgetFromClassChecked(GameOverlayClass, GameOverlay);
	CreateWidgetFromClassChecked(PlayerMenuClass, PlayerMenuWidget);

	UE_LOG(LogThresholdUI, Display, TEXT("APlayerHUD %s created starting widgets"), *GetNameSafe(this))
}

template <typename WidgetType>
void APlayerHUD::CreateWidgetFromClassChecked(TSubclassOf<WidgetType> WidgetClass, WidgetType*& WidgetVariable)
{
	if (!WidgetClass)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("APlayerHUD::CreateWidgets() failed on %s - invalid %s class"),
            *GetNameSafe(this), *GetNameSafe(WidgetType::StaticClass()))
		return;
	}
	WidgetVariable = CreateWidget<WidgetType>(GetOwningPlayerController(), WidgetClass);
	if (!WidgetVariable)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("APlayerHUD::CreateWidgets() failed on %s - failed to create widget of type %s"),
            *GetNameSafe(this), *GetNameSafe(WidgetClass))
		return;
	}
	WidgetVariable->AddToViewport();
}

