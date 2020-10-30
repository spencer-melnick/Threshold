// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/PlayerMenu.h"
#include "ThresholdUI/Widgets/InventoryMenu.h"


// UPlayerMenuWidget

UPlayerMenuWidget::UPlayerMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}



// Engine overrides

void UPlayerMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}



// Initialization

void UPlayerMenuWidget::OnPlayerStateInitialized()
{
	InventoryMenu->OnPlayerStateInitialized();

	InitializeInputComponent();
	UnregisterInputComponent();
	SetupInputComponent();
}



// Input binding

void UPlayerMenuWidget::SetupInputComponent()
{
	DECLARE_DELEGATE_OneParam(FMoveCursorDelegate, ESelectionDirection);
	check(InputComponent);

	// Bind cursor movements to input actions
	InputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorLeft"), EInputEvent::IE_Pressed, this,
		&UPlayerMenuWidget::MoveCursor, ESelectionDirection::Left);
	InputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorRight"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Right);
	InputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorUp"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Up);
	InputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorDown"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Down);

	
	// Hook child widgets into this control
	InventoryMenu->InitializeSelection(this);

	// Try to select the initial widget
	ISelectionController::TrySelectWidget(InventoryMenu);
}

void UPlayerMenuWidget::EnableInput()
{
	if (bInputEnabled)
	{
		return;
	}
	bInputEnabled = true;
	
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer || !InputComponent)
	{
		return;
	}

	// Unregister our input component to make sure it's only on the stack once
	UnregisterInputComponent();
	RegisterInputComponent();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	OwningPlayer->SetInputMode(InputMode);
	SnapCursorToViewCenter();
}

void UPlayerMenuWidget::DisableInput()
{
	if (!bInputEnabled)
	{
		return;
	}
	bInputEnabled = false;
	UnregisterInputComponent();
}

void UPlayerMenuWidget::MoveCursor(ESelectionDirection Direction)
{
	MoveSelection(Direction);

	APlayerController* Controller = GetOwningPlayer();
	if (Controller && Controller->bShowMouseCursor)
	{
		Controller->bShowMouseCursor = false;
	}
}



// Widget overrides

FReply UPlayerMenuWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (bInputEnabled && !InMouseEvent.GetCursorDelta().IsNearlyZero())
	{
		APlayerController* Controller = GetOwningPlayer();
		if (Controller && !Controller->bShowMouseCursor)
		{
			Controller->bShowMouseCursor = true;
		}
		
		if (!Reply.IsEventHandled())
		{
			// If the BP event didn't handle the event, then we need to let UMG know that we did
			return FReply::Handled();
		}	
	}

	return Reply;
}




// Widget controls

void UPlayerMenuWidget::EnableWidget()
{
	SetVisibility(ESlateVisibility::Visible);
	EnableInput();
}

void UPlayerMenuWidget::DisableWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
	DisableInput();
}



// Selection controller overrides

bool UPlayerMenuWidget::TrySelectWidget(FSelectableWidgetReference Widget, ESelectionDirection FromSelectionDirection)
{
	
	if (FromSelectionDirection == ESelectionDirection::None)
	{
		// Selection direction none is caused by mouse selection
		APlayerController* Controller = GetOwningPlayer();
		if (!Controller || !Controller->bShowMouseCursor)
		{
			// Skip if we're trying to select using a mouse but the cursor isn't visible yet
			return false;
		}
	}

	return ISelectionController::TrySelectWidget(Widget, FromSelectionDirection);
}




// Helper functions

void UPlayerMenuWidget::SnapCursorToViewCenter()
{
	APlayerController* Controller = GetOwningPlayer();
	if (!Controller)
	{
		return;
	}

	FIntPoint ViewportSize;
	Controller->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	Controller->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
}

