﻿// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/HUD/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "ThresholdUI.h"
#include "ThresholdUI/Widgets/PlayerMenu.h"


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
	Status = NewStatus;

	switch (Status)
	{
		case EPlayerHUDStatus::WorldView:
			HideWidgetChecked(PlayerMenuWidget);
			break;

		case EPlayerHUDStatus::PlayerMenuActive:
			ShowWidgetChecked(PlayerMenuWidget);
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



// Initialization

void APlayerHUD::OnPlayerStateInitialized()
{
	if (PlayerMenuWidget)
	{
		PlayerMenuWidget->OnPlayerStateInitialized();
	}
}




// Helper functions

void APlayerHUD::CreateWidgets()
{
	if (!PlayerMenuClass)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("APlayerHUD::CreateWidgets() failed on %s - invalid player menu class"),
			*GetNameSafe(this))
		return;
	}

	PlayerMenuWidget = CreateWidget<UPlayerMenuWidget>(GetOwningPlayerController(), PlayerMenuClass);

	if (!PlayerMenuWidget)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("APlayerHUD::CreateWidgets() failed on %s - failed to create widget of type %s"),
            *GetNameSafe(this), *GetNameSafe(PlayerMenuClass))
		return;
	}

	PlayerMenuWidget->AddToViewport();

	UE_LOG(LogThresholdUI, Display, TEXT("APlayerHUD %s created starting widgets"), *GetNameSafe(this))
}

void APlayerHUD::HideWidgetChecked(UUserWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void APlayerHUD::ShowWidgetChecked(UUserWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	Widget->SetVisibility(ESlateVisibility::Visible);
}