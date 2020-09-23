// Copyright (c) 2020 Spencer Melnick

#include "InventoryItem.h"



// FInventoryItem

FInventoryItem* FInventoryItem::Copy() const
{
	const UScriptStruct* ScriptStruct = GetScriptStruct();
	check(ScriptStruct);

	// Allocate a new struct of the correct size and copy our data
	FInventoryItem* ItemCopy = static_cast<FInventoryItem*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));
	FMemory::Memcpy(ItemCopy, this, ScriptStruct->GetStructureSize());
	return ItemCopy;
}




// FInventoryItemHandle

bool FInventoryItemHandle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	TCheckedObjPtr<UScriptStruct> ScriptStruct;

	if (Ar.IsSaving())
	{
		// Try to get the script struct if we have a valid object
		if (!ItemPointer)
		{
			ScriptStruct = nullptr;
		}
		else
		{
			ScriptStruct = ItemPointer->GetScriptStruct();
		}

		Ar << ScriptStruct;

		if (ScriptStruct.IsValid() && ScriptStruct->StructFlags & STRUCT_NetSerializeNative)
		{
			// If our struct is valid and supports network serialization, let it serialize itself
			return ScriptStruct->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, ItemPointer.Get());
		}

		// We sent an empty struct across the network - this should be okay!
		bOutSuccess = true;
		return true;
	}
	else if (Ar.IsLoading())
	{
		Ar << ScriptStruct;

		if (!ScriptStruct.IsValid())
		{
			// If we received an empty object, dump our existing object
			ItemPointer.Reset();
			bOutSuccess = true;
			return true;
		}
		
		if (!ItemPointer || ScriptStruct.Get() != ItemPointer->GetScriptStruct())
		{
			// Struct type has changed - allocate memory for our new struct
			FInventoryItem* ItemData = static_cast<FInventoryItem*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));
			ScriptStruct->InitializeStruct(ItemData);
			ItemPointer = TSharedPtr<FInventoryItem>(ItemData);
		}

		// Let the struct deserialize itself
		return ScriptStruct->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, ItemPointer.Get());
	}

	// Should be unreachable
	bOutSuccess = false;
	return false;
}



// FSimpleInventoryItem

bool FSimpleInventoryItem::operator==(const FInventoryItem& Other) const
{
	if (!Super::operator==(Other))
	{
		// Check that our struct types match
		return false;
	}

	// Compare gameplay tags
	const FSimpleInventoryItem& OtherUnique = static_cast<const FSimpleInventoryItem&>(Other);
	return GameplayTags == OtherUnique.GameplayTags;
}


bool FSimpleInventoryItem::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	// Serialize our gameplay tags
	return GameplayTags.NetSerialize(Ar, Map, bOutSuccess);
}
