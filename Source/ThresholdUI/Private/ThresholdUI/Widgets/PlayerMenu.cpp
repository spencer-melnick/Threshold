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
	TrySelectWidget(InventoryMenu);
}

void UPlayerMenuWidget::EnableInput()
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer || !InputComponent)
	{
		return;
	}

	// Pop our input component to make sure it's only on the stack once
	OwningPlayer->PopInputComponent(InputComponent);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	OwningPlayer->SetInputMode(InputMode);
	OwningPlayer->bShowMouseCursor = true;
	RegisterInputComponent();
}

void UPlayerMenuWidget::DisableInput()
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer || !InputComponent)
	{
		return;
	}

	UnregisterInputComponent();
}

void UPlayerMenuWidget::MoveCursor(ESelectionDirection Direction)
{
	MoveSelection(Direction);
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

