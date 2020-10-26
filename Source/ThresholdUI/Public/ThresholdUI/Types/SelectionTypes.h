// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"


// Forward declarations

class ISelectableWidget;



UENUM()
enum class ESelectionDirection : uint8
{
	Up,
    Down,
    Left,
    Right,
    None
};

using FSelectableWidgetReference = TScriptInterface<ISelectableWidget>;
