// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI.h"

IMPLEMENT_GAME_MODULE(FThresholdUIModule, ThresholdUI);

DEFINE_LOG_CATEGORY(LogThresholdUI);

#define LOCTEXT_NAMESPACE "ThresholdUI"

void FThresholdUIModule::StartupModule()
{
	UE_LOG(LogThresholdUI, Display, TEXT("ThresholdUI: Module Started"));
}

void FThresholdUIModule::ShutdownModule()
{
	UE_LOG(LogThresholdUI, Display, TEXT("ThresholdUI: Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE

