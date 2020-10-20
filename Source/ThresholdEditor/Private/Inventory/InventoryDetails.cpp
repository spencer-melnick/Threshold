// Copyright (c) 2020 Spencer Melnick

#include "InventoryDetails.h"

#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilder.h"
#include "UObject/StructOnScope.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/ItemTypes/ItemTypeBase.h"
#include "Inventory/DataTypes/ItemData.h"


TSharedRef<IPropertyTypeCustomization> FInventoryItemDetails::MakeInstance()
{
	return MakeShareable(new FInventoryItemDetails());
}

void FInventoryItemDetails::CustomizeHeader(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText TypeText = FText::FromString(TEXT("Inventory Item"));

	HeaderRow.NameContent()[
		InPropertyHandle->CreatePropertyNameWidget(TypeText)
	];
}

void FInventoryItemDetails::CustomizeChildren(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Store reference to property handle
	PropertyHandle = InPropertyHandle;	

	// Reference the parent and child layout
	ChildBuilder = &InChildBuilder;
	ParentBuilder = &ChildBuilder->GetParentCategory().GetParentLayout();

	// Update the struct display
	UpdateDataStruct();

	// Try to get the type property handle
	TypePropertyHandle = PropertyHandle->GetChildHandle(TEXT("Type"));

	if (!TypePropertyHandle)
	{
		return;
	}

	ChildBuilder->AddProperty(TypePropertyHandle.ToSharedRef());

	// Add a pre type change delegate
	FSimpleDelegate PreTypeChangeDelegate;
	PreTypeChangeDelegate.BindSP(this, &FInventoryItemDetails::PreTypeChange);
	TypePropertyHandle->SetOnPropertyValuePreChange(PreTypeChangeDelegate);

	// Add a post type change delegate
	FSimpleDelegate TypeChangeDelegate;
	TypeChangeDelegate.BindSP(this, &FInventoryItemDetails::TypeChange);
	TypePropertyHandle->SetOnPropertyValueChanged(TypeChangeDelegate);
}

FInventoryItem* FInventoryItemDetails::GetItem() const
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

	return static_cast<FInventoryItem*>(DataPointer);
}

void FInventoryItemDetails::NotifyPropertyChange() const
{
	if (!PropertyHandle.IsValid())
	{
		return;
	}

	PropertyHandle->NotifyPostChange();
}

void FInventoryItemDetails::PreTypeChange()
{
	if (!TypePropertyHandle.IsValid())
	{
		return;
	}

	// Try to store the old item type
	UObject* CurrentItemType;
	
	if (TypePropertyHandle->GetValue(CurrentItemType) != FPropertyAccess::Success)
	{
		// On failure to access the stashed type, set the stashed type to null
		StashedItemType = nullptr;
	}
	else
	{
		StashedItemType = Cast<UInventoryItemTypeBase>(CurrentItemType);
	}
}

void FInventoryItemDetails::TypeChange()
{
	FInventoryItem* Item = GetItem();

	if (!Item || !TypePropertyHandle.IsValid())
	{
		return;
	}

	UObject* NewItemObject;
	UInventoryItemTypeBase* NewItemType;
	if (TypePropertyHandle->GetValue(NewItemObject) != FPropertyAccess::Success)
	{
		NewItemType = nullptr;
	}
	else
	{
		NewItemType = Cast<UInventoryItemTypeBase>(NewItemObject);
	}

	// Simulate usage of the setter
	Item->Type = StashedItemType.Get();
	Item->SetType(NewItemType);

	// Update the internal struct access
	UpdateDataStruct();

	// Refresh the builder view
	NotifyPropertyChange();
	ChildBuilder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
}



void FInventoryItemDetails::UpdateDataStruct()
{
	if (!ChildBuilder)
	{
		return;
	}
	
	// Access the item handle from the property
	FInventoryItem* Item = GetItem();

	if (!Item || !Item->Data.IsValid() || !Item->Data->GetScriptStruct())
	{
		return;
	}

	UScriptStruct* ItemDataType = Item->Data->GetScriptStruct();
	
	// Add access to the internal data struct
	const TSharedRef<FStructOnScope> InternalStruct = MakeShared<FStructOnScope>(ItemDataType, reinterpret_cast<uint8*>(Item->Data.Get()));
	IDetailPropertyRow* StructProperty = ChildBuilder->AddExternalStructure(InternalStruct);

	// Set up delegate to mark property as dirty when internal struct changes
	FSimpleDelegate NotifyPropertyChangeDelegate;
	NotifyPropertyChangeDelegate.BindSP(this, &FInventoryItemDetails::NotifyPropertyChange);
	StructProperty->GetPropertyHandle()->SetOnChildPropertyValueChanged(NotifyPropertyChangeDelegate);
}

