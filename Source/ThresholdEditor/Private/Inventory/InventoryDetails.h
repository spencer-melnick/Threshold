// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"


// Forward declarations

class IDetailGroup;
struct FInventoryItemHandle;



/**
 * Class to display the properties of an inventory item in the editor for debugging
 */
class FInventoryHandleDetails : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(::TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(::TSharedRef<IPropertyHandle, ESPMode::Fast> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
};
