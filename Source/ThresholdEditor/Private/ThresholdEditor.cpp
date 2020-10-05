// Copyright (c) 2020 Spencer Melnick

#include "ThresholdEditor.h"

#include "Inventory/InventoryDetails.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FThresholdEditorModule, ThresholdEditor);

DEFINE_LOG_CATEGORY(LogThresholdEditor);

#define LOCTEXT_NAMESPACE "ThresholdEditor"

void FThresholdEditorModule::StartupModule()
{
	UE_LOG(LogThresholdEditor, Display, TEXT("ThresholdEditor: Module Started"));

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("InventoryItem", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInventoryItemDetails::MakeInstance));
}

void FThresholdEditorModule::ShutdownModule()
{
	UE_LOG(LogThresholdEditor, Display, TEXT("ThresholdEditor: Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE
