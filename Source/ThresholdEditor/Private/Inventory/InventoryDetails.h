// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"


// Forward declarations

class IDetailGroup;
class IDetailLayoutBuilder;
struct FInventoryItem;
class UInventoryItemTypeBase;



/**
 * Class to display the properties of an inventory item
 */
class FInventoryItemDetails : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle, ESPMode::Fast> InPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	UScriptStruct* GetType(TSharedPtr<FString> TypeString) const;
	TSharedPtr<FString> GetTypeString(UScriptStruct* Type) const;

protected:
	FInventoryItem* GetItem() const;
	void NotifyPropertyChange() const;
	void PreTypeChange();
	void TypeChange();
	void UpdateDataStruct();

private:
	IDetailLayoutBuilder* ParentBuilder = nullptr;
	IDetailChildrenBuilder* ChildBuilder = nullptr;
	
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<IPropertyHandle> TypePropertyHandle;
	
	TWeakObjectPtr<UInventoryItemTypeBase> StashedItemType = nullptr;
};
