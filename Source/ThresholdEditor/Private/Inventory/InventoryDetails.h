// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "Types/SlateEnums.h"


// Forward declarations

class IDetailGroup;
class IDetailLayoutBuilder;
struct FInventoryItemHandle;



/**
 * Class to display the properties of an inventory item in the editor for debugging
 */
class FInventoryHandleDetails : public IPropertyTypeCustomization
{
public:
	FInventoryHandleDetails();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	void OnTypeSelection(TSharedPtr<FString> NewType, ESelectInfo::Type SelectInfo);
	void NotifyPropertyChange();

	UScriptStruct* GetType(TSharedPtr<FString> TypeString) const;
	TSharedPtr<FString> GetTypeString(UScriptStruct* Type) const;

protected:
	TArray<TSharedPtr<FString>> TypeStrings;
	TMap<TSharedPtr<FString>, UScriptStruct*> TypeMappings;

private:
	FInventoryItemHandle* Handle = nullptr;
	IDetailLayoutBuilder* ParentBuilder = nullptr;
	TSharedPtr<IPropertyHandle> ChildPropertyHandle;
};
