// Copyright (c) 2020 Spencer Melnick

#include "InventoryDetails.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "UObject/SoftObjectPtr.h"
#include "IDetailChildrenBuilder.h"
#include "UObject/Class.h"
#include "Player/Inventory/InventoryItem.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"

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

	FInventoryItemHandle& ItemHandle = *static_cast<FInventoryItemHandle*>(DataPointer);

	AddCheckBoxDisplay(ANSI_TO_TCHAR("CanStack"), ItemHandle->CanStack(), ChildBuilder, CustomizationUtils);
	AddCheckBoxDisplay(ANSI_TO_TCHAR("CanHaveDuplicates"), ItemHandle->CanHaveDuplicates(), ChildBuilder, CustomizationUtils);

	const FText TagText = FText::FromString(ItemHandle->GetGameplayTags().ToStringSimple(true));
	AddTextDisplay(ANSI_TO_TCHAR("GameplayTags"), TagText, ChildBuilder, CustomizationUtils);

	const FText ActorPathText = FText::FromString(ItemHandle->GetPreviewActorClass().ToString());
	AddTextDisplay(ANSI_TO_TCHAR("PreviewActorClassPath"), ActorPathText, ChildBuilder, CustomizationUtils);
}

void FInventoryHandleDetails::AddCheckBoxDisplay(const FString& Name, bool bChecked, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText DisplayText = FText::FromString(Name);

	ChildBuilder.AddCustomRow(DisplayText)
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

void FInventoryHandleDetails::AddTextDisplay(const FString& Name, const FText& Text, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText LabelText = FText::FromString(Name);

	ChildBuilder.AddCustomRow(LabelText)
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

