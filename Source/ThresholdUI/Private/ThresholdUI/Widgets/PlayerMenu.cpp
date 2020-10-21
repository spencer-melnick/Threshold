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

