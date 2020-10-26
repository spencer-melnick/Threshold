// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdUI/Types/SelectionTypes.h"
#include "SelectionController.generated.h"



// Forward declarations

class ISelectableWidget;



UINTERFACE()
class USelectionController : public UInterface
{
	GENERATED_BODY()
};

/**
 * Object that drives the control of any selectable widgets
 */
class ISelectionController
{
	GENERATED_BODY()

public:
	/**
	 * Try to select the widget, as if from the specified direction
	 * @return True if the widget was successfully selected, false otherwise
	 */
	virtual bool TrySelectWidget(FSelectableWidgetReference Widget, ESelectionDirection FromSelectionDirection = ESelectionDirection::None);

	/**
	 * Returns the currently selected widget
	 */
	virtual FSelectableWidgetReference GetSelectedWidget() const;

	/**
	 * Attempts to move the selection in the direction given
	 * @return True if the selection could move successfully, false otherwise
	 */
	virtual bool MoveSelection(const ESelectionDirection SelectionDirection);

protected:

	/**
	 * Any implementers simply need to implement this function and have it return a pointer to an
	 * ISelectableWidgetInterface pointer if using default functionality. The default controls of the selection
	 * controller will handle setting everything using this pointer. If this isn't implemented, then the default
	 * controls will fail.
	 */
	virtual FSelectableWidgetReference* GetSelectedWidgetPointer() { unimplemented(); return nullptr; }

	virtual const FSelectableWidgetReference* GetSelectedWidgetPointer() const { unimplemented(); return nullptr; }

private:

	void SetSelectedWidgetInternal(FSelectableWidgetReference Widget) { GetSelectedWidgetInternal() = Widget; }

	FSelectableWidgetReference& GetSelectedWidgetInternal();
	
	const FSelectableWidgetReference& GetSelectedWidgetInternal() const;
};
