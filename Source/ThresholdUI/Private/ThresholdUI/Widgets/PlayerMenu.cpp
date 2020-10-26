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
}



// Input binding

void UPlayerMenuWidget::SetupInputComponent(UInputComponent* InInputComponent)
{
	DECLARE_DELEGATE_OneParam(FMoveCursorDelegate, ESelectionDirection);
	check(InInputComponent);

	// Bind cursor movements to input actions
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorLeft"), EInputEvent::IE_Pressed, this,
		&UPlayerMenuWidget::MoveCursor, ESelectionDirection::Left);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorRight"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Right);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorUp"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Up);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorDown"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Down);

	
	// Hook child widgets into this control
	InventoryMenu->InitializeSelection(this);

	// Try to select the initial widget
	TrySelectWidget(InventoryMenu);
}

void UPlayerMenuWidget::MoveCursor(ESelectionDirection Direction)
{
	MoveSelection(Direction);
}

