// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "PlayerWidget.generated.h"



/**
 * Interface used for common player widgets
 */
UINTERFACE()
class UPlayerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class IPlayerWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void OnPlayerStateInitialized() = 0;
};
