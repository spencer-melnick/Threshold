// Copyright (c) 2020 Spencer Melnick

#include "InventoryDetails.h"

#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "UObject/SoftObjectPtr.h"
#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "UObject/Class.h"
#include "Player/Inventory/InventoryItem.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "UObject/StructOnScope.h"

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
	
	const FText TypeText = FText::FromString(ANSI_TO_TCHAR("Inventory Item Handle"));

	HeaderRow.NameContent()[
		PropertyHandle->CreatePropertyNameWidget(TypeText)
	];
}

void FInventoryHandleDetails::CustomizeChildren(TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
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

	// Access the item handle from the property
	FInventoryItemHandle& ItemHandle = *static_cast<FInventoryItemHandle*>(DataPointer);

	// Add access to the internal struct
	const TSharedRef<FStructOnScope> InternalStruct = MakeShared<FStructOnScope>(ItemHandle->GetScriptStruct(), reinterpret_cast<uint8*>(&(*ItemHandle)));
	ChildBuilder.AddExternalStructure(InternalStruct);
}
