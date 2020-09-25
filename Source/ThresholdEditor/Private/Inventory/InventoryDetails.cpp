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
	// ChildBuilder.GetParentCategory().GetParentLayout().

	/*
	// Add delegate to refresh display when struct is changed
	FSimpleDelegate ValueChangedDelegate;
	ValueChangedDelegate.BindLambda([&ChildBuilder]()
	{
		IDetailCategoryBuilder& CategoryBuilder = ChildBuilder.GetParentCategory();
		IDetailLayoutBuilder& LayoutBuilder = CategoryBuilder.GetParentLayout();
		LayoutBuilder.ForceRefreshDetails();
	});

	// Bind the delegate to all struct properties;
	for (TSharedPtr<IPropertyHandle>& StructPropertyHandle : StructHandles)
	{
		StructPropertyHandle->SetOnPropertyValueChanged(ValueChangedDelegate);	
	}
	*/
}

void FInventoryHandleDetails::AddCheckBoxDisplay(const FString& Name, bool bChecked, IDetailGroup& DetailGroup, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText DisplayText = FText::FromString(Name);

	DetailGroup.AddWidgetRow()
        .NameWidget[
            SNew(STextBlock)
            .Text(DisplayText)
            .Font(CustomizationUtils.GetRegularFont())
        ]
        .ValueWidget[
            SNew(SCheckBox)
            .IsEnabled(false)
            .IsChecked(bChecked)
        ];
}

void FInventoryHandleDetails::AddTextDisplay(const FString& Name, const FText& Text, IDetailGroup& DetailGroup, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText LabelText = FText::FromString(Name);

	DetailGroup.AddWidgetRow()
		.NameWidget[
			SNew(STextBlock)
			.Text(LabelText)
			.Font(CustomizationUtils.GetRegularFont())
		]
		.ValueWidget[
			SNew(STextBlock)
			.Text(Text)
			.Font(CustomizationUtils.GetRegularFont())
		];
}

void FInventoryHandleDetails::AddInterfaceProperties(FInventoryItemHandle& ItemHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Add all the interface properties as displays
	IDetailGroup& InterfaceGroup = ChildBuilder.AddGroup(ANSI_TO_TCHAR("Interface Properties"), FText::FromString(ANSI_TO_TCHAR("Interface Properties")));

	AddCheckBoxDisplay(ANSI_TO_TCHAR("Can Stack"), ItemHandle->CanStack(), InterfaceGroup, CustomizationUtils);
	AddCheckBoxDisplay(ANSI_TO_TCHAR("Can Have Duplicates"), ItemHandle->CanHaveDuplicates(), InterfaceGroup, CustomizationUtils);

	const FText TagText = FText::FromString(ItemHandle->GetGameplayTags().ToStringSimple(true));
	AddTextDisplay(ANSI_TO_TCHAR("Gameplay Tags"), TagText, InterfaceGroup, CustomizationUtils);

	const FText ActorPathText = FText::FromString(ItemHandle->GetPreviewActorClass().ToString());
	AddTextDisplay(ANSI_TO_TCHAR("Preview Actor"), ActorPathText, InterfaceGroup, CustomizationUtils);
}

