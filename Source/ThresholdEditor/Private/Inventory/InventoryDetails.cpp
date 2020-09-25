// Copyright (c) 2020 Spencer Melnick

#include "InventoryDetails.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "UObject/Class.h"
#include "Player/Inventory/InventoryItem.h"

TSharedRef<IPropertyTypeCustomization> FInventoryHandleDetails::MakeInstance()
{
	return MakeShareable(new FInventoryHandleDetails());
}

void FInventoryHandleDetails::CustomizeHeader(TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	void* DataPointer;
	if (PropertyHandle->GetValueData(DataPointer) == FPropertyAccess::Fail)
	{
		return;
	}

	FInventoryItemHandle& ItemHandle = *static_cast<FInventoryItemHandle*>(DataPointer);
	UScriptStruct* ScriptStruct = ItemHandle->GetScriptStruct();
	FText TypeText = FText::FromString(ScriptStruct->GetName());

	HeaderRow.NameContent()[
		PropertyHandle->CreatePropertyNameWidget(TypeText)
	];
}

void FInventoryHandleDetails::CustomizeChildren(::TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	
}

