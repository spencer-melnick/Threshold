// Copyright (c) 2020 Spencer Melnick

#include "InventorySystem.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FInventorySystemModule, InventorySystem);

DEFINE_LOG_CATEGORY(LogInventorySystem);

#define LOCTEXT_NAMESPACE "InventorySystem"

void FInventorySystemModule::StartupModule()
{
	UE_LOG(LogInventorySystem, Display, TEXT("InventorySystem: Module Started"));
}

void FInventorySystemModule::ShutdownModule()
{
	UE_LOG(LogInventorySystem, Display, TEXT("InventorySystem: Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE
