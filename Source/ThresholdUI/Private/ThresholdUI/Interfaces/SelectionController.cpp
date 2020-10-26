// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Interfaces/SelectionController.h"
#include "ThresholdUI/Interfaces/SelectableWidget.h"
#include "ThresholdUI/Blueprint/SelectionFunctionLibrary.h"



// ISelectionController

bool ISelectionController::TrySelectWidget(FSelectableWidgetReference Widget, ESelectionDirection FromSelectionDirection)
{
	if (!Widget)
	{
		return false;
	}

	// Traverse down the hierarchy to get the next selected widget
	const FSelectableWidgetReference NewWidget = Widget->TrySelect(FromSelectionDirection);
	if (!NewWidget)
	{
		return false;
	}

	FSelectableWidgetReference CurrentWidget = GetSelectedWidgetInternal();
	if (CurrentWidget)
	{
		// Notify the previous widget that it was deselected
		CurrentWidget->OnDeselected();
	}

	SetSelectedWidgetInternal(NewWidget);
	return true;
}

FSelectableWidgetReference ISelectionController::GetSelectedWidget() const
{
	return GetSelectedWidgetInternal();
}

bool ISelectionController::MoveSelection(const ESelectionDirection SelectionDirection)
{
	FSelectableWidgetReference SelectedWidget = GetSelectedWidget();
	FSelectableWidgetReference NextWidget = nullptr;

	while (!NextWidget && SelectedWidget)
	{
		// Traverse up the hierarchy looking for a valid adjacent widget
		NextWidget = SelectedWidget->GetAdjacentWidget(SelectionDirection);
		SelectedWidget = SelectedWidget->GetParentWidget();
	}

	if (!NextWidget)
	{
		return false;
	}

	return TrySelectWidget(NextWidget, USelectionFunctionLibrary::FindOppositeDirection(SelectionDirection));
}





// Internal functions

FSelectableWidgetReference& ISelectionController::GetSelectedWidgetInternal()
{
	FSelectableWidgetReference* WidgetPointer = GetSelectedWidgetPointer();
	check(WidgetPointer);
	return *WidgetPointer;
}

const FSelectableWidgetReference& ISelectionController::GetSelectedWidgetInternal() const
{
	const FSelectableWidgetReference* WidgetPointer = GetSelectedWidgetPointer();
	check(WidgetPointer);
	return *WidgetPointer;
}

