// Copyright (c) 2020 Spencer Melnick

#include "InventoryItem.h"
#include "InventorySystem.h"
#include "ItemTypes/ItemType.h"
#include "DataTypes/ItemData.h"


// FInventoryItem

// Property accessors

FText FInventoryItem::GetName() const
{
	if (!Type)
	{
		return FText();
	}

	return Type->GetItemName(Data);
}

FText FInventoryItem::GetDescription() const
{
	if (!Type)
	{
		return FText();
	}

	return Type->GetItemDescription(Data);
}

TSoftClassPtr<AActor> FInventoryItem::GetPreviewActorClass() const
{
	if (!Type)
	{
		return nullptr;
	}

	return Type->GetPreviewActorClass(Data);
}



// Data accessors

void FInventoryItem::SetType(UItemTypeBase* NewType)
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
		// Allocate and initialize item data based on the new data type if it changed
		FItemDataBase* NewItemData = static_cast<FItemDataBase*>(FMemory::Malloc(NewItemDataType->GetStructureSize()));
		NewItemDataType->InitializeStruct(NewItemData);

		// Store our new item data
		Data = TSharedPtr<FItemDataBase>(NewItemData);
	}
}




// Serialization

bool FInventoryItem::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	bool bStaticItemType;
	TCheckedObjPtr<UItemTypeBase> SerializedItemType;
	TSubclassOf<UItemTypeBase> SerializeItemTypeClass;

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
			SerializeItemTypeClass = Type->GetClass();
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
		Ar << SerializeItemTypeClass;
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
			if (Type && SerializeItemTypeClass != Type->GetClass())
			{
				// Create a new type object if it's different than our current type
				UItemTypeBase* NewItemType = NewObject<UItemTypeBase>(static_cast<UObject*>(GetTransientPackage()), SerializeItemTypeClass);
				SetType(NewItemType);
			}

			// Deserialize the mutable type data
			Type->NetSerialize(Ar, PackageMap, bOutSuccess);
		}

		UScriptStruct* ItemDataType = Type->GetItemDataType();

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
	TCheckedObjPtr<UItemTypeBase> ItemType;

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

