// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(LogThresholdUI, All, All)

class THRESHOLDUI_API FThresholdUIModule : public IModuleInterface
{
	public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};


/**
 * CHECK_WIDGET_STATEMENT and CHECK_WIDGET_STATEMENT_RETURN
 * are simple macros that exit the function if this is an editor build, and the statement x is false, with the provided
 * optional return value. Its designed to make it easy to check if bound widgets exist in the editor build (we're
 * assuming they exist in a packaged build, since the widget Blueprint shouldn't be able to compile anyways)
 */

#if WITH_EDITOR
	#define CHECK_WIDGET_STATEMENT_RETURN(x, y) \
		if (!(x)) \
		{ \
			return y; \
		}
	#define CHECK_WIDGET_STATEMENT(x) CHECK_WIDGET_STATEMENT_RETURN(x, void())
#else
	#define CHECK_WIDGET_STATEMENT_RETURN(x, y)
	#define CHECK_WIDGET_STATEMENT(x)
#endif
