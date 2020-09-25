// Copyright (c) 2020 Spencer Melnick

#include "InventoryDetails.h"

#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "UObject/SoftObjectPtr.h"
#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilder.h"
#include "UObject/Class.h"
#include "Player/Inventory/InventoryItem.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "UObject/StructOnScope.h"
#include "Widgets/Input/STextComboBox.h"
#include "Threshold/Player/Inventory/Items/InventoryTableItem.h"


FInventoryHandleDetails::FInventoryHandleDetails()
{
	TArray<UScriptStruct*> ItemTypes = { FInventoryItem::StaticStruct(), FSimpleInventoryItem::StaticStruct(), FInventoryTableItem::StaticStruct() };

	for (UScriptStruct* Type : ItemTypes)
	{
		TSharedPtr<FString> TypeString;

		if (Type == FInventoryItem::StaticStruct())
		{
			TypeString = MakeShareable<FString>(new FString(TEXT("None")));
		}
		else
		{
			TypeString = MakeShareable<FString>(new FString(Type->GetName()));
		}
		
		TypeStrings.Add(TypeString);
		TypeMappings.Add(TypeString, Type);
	}
}


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
	
	const FText TypeText = FText::FromString(TEXT("Inventory Item Handle"));

	void* DataPointer;
	if (PropertyHandle->GetValueData(DataPointer) == FPropertyAccess::Fail)
	{
		return;
	}

	// Access the item handle from the property
	FInventoryItemHandle& ItemHandle = *static_cast<FInventoryItemHandle*>(DataPointer);

	HeaderRow.NameContent()[
		PropertyHandle->CreatePropertyNameWidget(TypeText)
	]
	.ValueContent()[
		SNew(STextComboBox)
		.OptionsSource(&TypeStrings)
		.InitiallySelectedItem(GetTypeString(ItemHandle->GetScriptStruct()))
		.OnSelectionChanged(this, &FInventoryHandleDetails::OnTypeSelection)
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
	Handle = static_cast<FInventoryItemHandle*>(DataPointer);

	if (!Handle->ItemPointer.IsValid())
	{
		return;
	}

	// Add access to the internal struct
	const TSharedRef<FStructOnScope> InternalStruct = MakeShared<FStructOnScope>((*Handle)->GetScriptStruct(), reinterpret_cast<uint8*>(Handle->ItemPointer.Get()));
	ChildBuilder.AddExternalStructure(InternalStruct);

	// Reference the parent layout
	ParentBuilder = &ChildBuilder.GetParentCategory().GetParentLayout();
}

void FInventoryHandleDetails::OnTypeSelection(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	UScriptStruct* NewType = GetType(NewSelection);

	if (!Handle || (*Handle)->GetScriptStruct() == NewType)
	{
		// If the handle is invalid, or it has the same type as before, skip
		return;
	}

	FInventoryItem* NewItem = static_cast<FInventoryItem*>(FMemory::Malloc(NewType->GetStructureSize()));
	NewType->InitializeStruct(NewItem);

	// Assign the handle to the new struct
	(*Handle).ItemPointer = TSharedPtr<FInventoryItem>(NewItem);

	// Refresh the layout
	if (ParentBuilder)
	{
		ParentBuilder->ForceRefreshDetails();
	}
}

TSharedPtr<FString> FInventoryHandleDetails::GetTypeString(UScriptStruct* Type) const
{
	const TSharedPtr<FString>* TypeName = TypeMappings.FindKey(Type);

	if (!TypeName)
	{
		// Return the default type string
		return TypeStrings[0];
	}

	return *TypeName;
}

UScriptStruct* FInventoryHandleDetails::GetType(TSharedPtr<FString> TypeString) const
{
	UScriptStruct* const* Type = TypeMappings.Find(TypeString);

	if (!Type)
	{
		// return default type
		return FInventoryItem::StaticStruct();
	}

	return *Type;
}


