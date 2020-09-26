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

void FInventoryHandleDetails::CustomizeHeader(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyHandle = InPropertyHandle;	
	const FText TypeText = FText::FromString(TEXT("Inventory Item"));

	// Access the item handle from the property
	FInventoryItemHandle* ItemHandle = GetItemHandle();

	if (!ItemHandle)
	{
		return;
	}

	HeaderRow.NameContent()[
		PropertyHandle->CreatePropertyNameWidget(TypeText)
	]
	.ValueContent()[
		SNew(STextComboBox)
		.OptionsSource(&TypeStrings)
		.InitiallySelectedItem(GetTypeString((*ItemHandle)->GetScriptStruct()))
		.OnSelectionChanged(this, &FInventoryHandleDetails::OnTypeSelection)
		.Font(CustomizationUtils.GetRegularFont())
	];
}

void FInventoryHandleDetails::CustomizeChildren(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Access the item handle from the property
	FInventoryItemHandle* ItemHandle = GetItemHandle();

	if (!ItemHandle || !ItemHandle->ItemPointer.IsValid())
	{
		return;
	}

	// Add access to the internal struct
	const TSharedRef<FStructOnScope> InternalStruct = MakeShared<FStructOnScope>((*ItemHandle)->GetScriptStruct(), reinterpret_cast<uint8*>(ItemHandle->ItemPointer.Get()));
	IDetailPropertyRow* StructProperty = ChildBuilder.AddExternalStructure(InternalStruct);

	// Set up delegate to mark property as dirty when internal struct changes
	FSimpleDelegate NotifyPropertyChangeDelegate;
	NotifyPropertyChangeDelegate.BindSP(this, &FInventoryHandleDetails::NotifyPropertyChange);
	StructProperty->GetPropertyHandle()->SetOnChildPropertyValueChanged(NotifyPropertyChangeDelegate);

	// Reference the parent layout
	ParentBuilder = &ChildBuilder.GetParentCategory().GetParentLayout();
}

void FInventoryHandleDetails::OnTypeSelection(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	UScriptStruct* NewType = GetType(NewSelection);
	FInventoryItemHandle* ItemHandle = GetItemHandle();

	if (!ItemHandle || (*ItemHandle)->GetScriptStruct() == NewType)
	{
		// If the handle has the same type as before, skip
		return;
	}

	// Create a new item handle
	FInventoryItem* NewItem = static_cast<FInventoryItem*>(FMemory::Malloc(NewType->GetStructureSize()));
	NewType->InitializeStruct(NewItem);
	FInventoryItemHandle NewHandle(NewItem);
	*ItemHandle = NewHandle;

	// Refresh the layout
	if (ParentBuilder)
	{
		ParentBuilder->ForceRefreshDetails();
	}

	PropertyHandle->SetValue(true);
	NotifyPropertyChange();
}

void FInventoryHandleDetails::NotifyPropertyChange()
{
	if (PropertyHandle.IsValid())
	{
		PropertyHandle->NotifyPostChange();
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

FInventoryItemHandle* FInventoryHandleDetails::GetItemHandle() const
{
	if (!PropertyHandle.IsValid() || !PropertyHandle->IsValidHandle())
	{
		return nullptr;
	}

	void* DataPointer;
	if (PropertyHandle->GetValueData(DataPointer) == FPropertyAccess::Fail)
	{
		return nullptr;
	}

	return static_cast<FInventoryItemHandle*>(DataPointer);
}

