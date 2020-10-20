// Copyright (c) 2020 Spencer Melnick

#include "Inventory/InventoryItem.h"
#include "InventorySystem.h"
#include "Inventory/ItemTypes/ItemTypeBase.h"
#include "Inventory/DataTypes/ItemData.h"


// FInventoryItem

// Property accessors

FText FInventoryItem::GetName() const
{
	if (!IsValid())
	{
		return FText();
	}

	return Type->GetItemName(Data);
}

FText FInventoryItem::GetDescription() const
{
	if (!IsValid())
	{
		return FText();
	}

	return Type->GetItemDescription(Data);
}

TSoftClassPtr<APreviewActor> FInventoryItem::GetPreviewActorClass() const
{
	if (!IsValid())
	{
		return nullptr;
	}

	return Type->GetPreviewActorClass(Data);
}

bool FInventoryItem::AllowsDuplicates() const
{
	if (!IsValid())
	{
		return false;
	}

	return Type->AllowsDuplicates();
}

bool FInventoryItem::AllowsStacking() const
{
	if (!IsValid())
	{
		return false;
	}

	return Type->AllowsStacking();
}

int32 FInventoryItem::AddToStack(const int32 Count)
{
	if (!IsValid() || !AllowsStacking())
	{
		return 0;
	}

	return Type->AddToStack(Data, Count);
}

int32 FInventoryItem::RemoveFromStack(const int32 Count)
{
	if (!IsValid() || !AllowsStacking())
	{
		return 0;
	}

	return Type->RemoveFromStack(Data, Count);
}

void FInventoryItem::SetStackCount(const int32 Count) const
{
	if (!IsValid() || !AllowsStacking())
	{
		return;
	}

	Type->SetStackCount(Data, Count);
}


int32 FInventoryItem::GetStackCount() const
{
	if (!IsValid() || !AllowsStacking())
	{
		return 0;
	}

	return Type->GetStackCount(Data);
}

bool FInventoryItem::IsValid() const
{
	if (!Type)
	{
		return false;
	}

	if (Type->GetItemDataType())
	{
		// Make sure our data type matches the expected type
		return Data.IsValid() && Type->GetItemDataType() == Data->GetScriptStruct();
	}

	// If we have require no data, then the item should be valid
	return true;
}




// Data accessors

void FInventoryItem::SetType(UInventoryItemTypeBase* NewType)
{
	if (Type == NewType)
	{
		return;
	}
	
	Type = NewType;

	if (!Type || !Type->GetItemDataType())
	{
		// Dump our item data if the item type or item data type becomes null
		Data.Reset();
		return;
	}

	UScriptStruct* NewItemDataType = Type->GetItemDataType();

	if (!Data.IsValid() || NewItemDataType != Data->GetScriptStruct())
	{
		Data = Type->CreateItemData();
	}
}




// Serialization

bool FInventoryItem::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	bool bStaticItemType;
	TCheckedObjPtr<UInventoryItemTypeBase> SerializedItemType;
	TSubclassOf<UInventoryItemTypeBase> SerializedItemTypeClass;

	if (Ar.IsSaving())
	{
		if (!Type)
		{
			UE_LOG(LogInventorySystem, Error, TEXT("FInventoryItem::NetSerialize failed - invalid item type"))
			bOutSuccess = false;
			return true;
		}

		bStaticItemType = Type->IsSupportedForNetworking();

		if (bStaticItemType)
		{
			// If our type is supported for networking, just replicate the object reference itself
			SerializedItemType = Type;
		}
		else
		{
			// Otherwise serialize the type class so we can create a new object
			SerializedItemTypeClass = Type->GetClass();
		}
	}

	// Serialize whether or not our item type is static
	Ar << bStaticItemType;

	if (bStaticItemType)
	{
		// Serialize the object reference if it's static
		Ar << SerializedItemType;
	}
	else
	{
		// Serialize the object class if it's not
		Ar << SerializedItemTypeClass;
	}

	if (Ar.IsSaving())
	{
		if (!bStaticItemType)
		{
			// If our type is mutable, have it serialize whatever additional data it needs
			Type->NetSerialize(Ar, PackageMap, bOutSuccess);
		}

		UScriptStruct* ItemDataType = Type->GetItemDataType();

		if (ItemDataType)
		{
			check(ItemDataType->GetCppStructOps()->HasNetSerializer());

			// Serialize data based on our item data type
			ItemDataType->GetCppStructOps()->NetSerialize(Ar, PackageMap, bOutSuccess, Data.Get());
		}
	}
	else if (Ar.IsLoading())
	{
		if (!bStaticItemType)
		{
			// Default to null if we don't have a type class
			TSubclassOf<UInventoryItemTypeBase> CurrentItemTypeClass = nullptr;

			if (Type)
			{
				// Get the current type class if we have one
				CurrentItemTypeClass = Type->GetClass(); 
			}
			
			if (SerializedItemTypeClass != CurrentItemTypeClass)
			{
				// Create a new type object if it's different than our current type

				UInventoryItemTypeBase* NewItemType = nullptr;
				
				if (SerializedItemTypeClass)
				{
					// Create a new object if the type is non-null (fall back to an empty object if it is)
					NewItemType = NewObject<UInventoryItemTypeBase>(static_cast<UObject*>(GetTransientPackage()), SerializedItemTypeClass);
				}
				
				SetType(NewItemType);
			}

			// Deserialize the mutable type data
			if (Type)
			{
				Type->NetSerialize(Ar, PackageMap, bOutSuccess);
			}
		}
		else
		{
			SetType(SerializedItemType.Get());
		}

		UScriptStruct* ItemDataType = nullptr;

		if (Type)
		{
			ItemDataType = Type->GetItemDataType();
		}

		if (ItemDataType)
		{
			check(ItemDataType->GetCppStructOps()->HasNetSerializer());
			
			// Serialize data based on our type if it's not null
			ItemDataType->GetCppStructOps()->NetSerialize(Ar, PackageMap, bOutSuccess, Data.Get());
		}
	}

	return true;
}

bool FInventoryItem::Serialize(FArchive& Ar)
{
	TCheckedObjPtr<UInventoryItemTypeBase> ItemType;

	if (Ar.IsSaving())
	{
		// On save store our item type
		ItemType = Type;
	}
	
	// Fortunately on normal serialization we can just serialize UObjects so we only need to deal with the item data
	Ar << ItemType;
	
	if (Ar.IsLoading())
	{
		if (!ItemType.IsValid())
		{
			UE_LOG(LogInventorySystem, Error, TEXT("FInventoryItem::Serialize failed - invalid item type"))
			SetType(nullptr);

			// Serialize an empty struct (this works for some reason?)
			// Based on MovieSceneEvalTemplateSerializer
			FInventoryItemDataBase BaseData;
			FInventoryItemDataBase::StaticStruct()->SerializeItem(Ar, &BaseData, nullptr);
			return true;
		}

		// On load set our item type and allocate new data if needed
		SetType(ItemType.Get());
	}
		
	if (Type && Type->GetItemDataType())
	{
		// If our expected data type isn't null, our data should be valid
		check(Data.IsValid());
		
		// Serialize the item data
		Type->GetItemDataType()->SerializeItem(Ar, Data.Get(), nullptr);
	}

	return true;
}

