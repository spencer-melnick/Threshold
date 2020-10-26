// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/PlayerMenu.h"
#include "Blueprint/WidgetTree.h"



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
	if (!WidgetTree)
	{
		return;
	}

	TArray<UWidget*> ChildWidgets;
	WidgetTree->GetAllWidgets(ChildWidgets);

	for (UWidget* Widget : ChildWidgets)
	{
		IPlayerWidgetInterface* PlayerWidget = Cast<IPlayerWidgetInterface>(Widget);

		if (PlayerWidget)
		{
			PlayerWidget->OnPlayerStateInitialized();
		}
	}
}



// Input binding

void UPlayerMenuWidget::SetupInputComponent(UInputComponent* InInputComponent)
{
	DECLARE_DELEGATE_OneParam(FMoveCursorDelegate, ESelectionDirection);
	check(InInputComponent);

	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorLeft"), EInputEvent::IE_Pressed, this,
		&UPlayerMenuWidget::MoveCursor, ESelectionDirection::Left);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorRight"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Right);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorUp"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Up);
	InInputComponent->BindAction<FMoveCursorDelegate>(TEXT("CursorDown"), EInputEvent::IE_Pressed, this,
        &UPlayerMenuWidget::MoveCursor, ESelectionDirection::Down);
}

void UPlayerMenuWidget::MoveCursor(ESelectionDirection Direction)
{
	MoveSelection(Direction);
}

