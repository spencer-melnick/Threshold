// Copyright (c) 2020 Spencer Melnick

#include "Inventory/DataTypes/ItemData.h"

FInventoryItemDataBase* FInventoryItemDataBase::Copy() const
{
	UScriptStruct* ScriptStruct = GetScriptStruct();
	if (!ScriptStruct)
	{
		return nullptr;
	}

	// Allocate a new struct and copy data from this struct
	FInventoryItemDataBase* NewData = static_cast<FInventoryItemDataBase*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));
	ScriptStruct->InitializeStruct(NewData);
	ScriptStruct->CopyScriptStruct(NewData, this);

	return NewData;
}

