// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "ThresholdUI/Types/SelectionTypes.h"
#include "SelectableWidget.generated.h"



// Forward declarations

class ISelectionController;



UINTERFACE()
class USelectableWidget : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for widgets that can be selected in a menu, either with a pointer or a controller
 */
class ISelectableWidget
{
	GENERATED_BODY()

public:

	/**
	 * Called when this widget is selected by a pointer
	 * @param FromSelectionDirection - Direction that the selection came from (usually the widget that was previously
	 * selected)
	 * @return The widget that should be selected
	 */
	virtual FSelectableWidgetReference TrySelect(const ESelectionDirection FromSelectionDirection) = 0;

	/**
	 * Should return the widget in the specified direction of this widget, or nullptr if there is no adjacent widget.
	 * If the returned widget is null, the controller should attempt to use the parent's adjacent widgets
	 */
	virtual FSelectableWidgetReference GetAdjacentWidget(const ESelectionDirection InSelectionDirection) const { return nullptr; }

	/**
	 * Returns the parent widget, if any
	 */
	virtual FSelectableWidgetReference GetParentWidget() const { return nullptr; }

	/**
	 * Called when this widget is activated, either via a click or controller
	 */
	virtual void OnSelectionActivated() { };

	/**
	 * Called when the widget is no longer selected
	 */
	virtual void OnDeselected() { };

	/**
	 * Should be called when this widget is initialized - sets the controller
	 */
	virtual void InitializeSelection(TScriptInterface<ISelectionController> Controller) { };
};
