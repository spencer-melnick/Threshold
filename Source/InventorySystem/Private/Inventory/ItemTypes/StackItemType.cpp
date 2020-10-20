// Copyright (c) 2020 Spencer Melnick

#include "Inventory/ItemTypes/StackItemType.h"
#include "Inventory/DataTypes/StackData.h"


// UInventoryStackItemType

bool UInventoryStackItemType::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, PackageMap, bOutSuccess);

	Ar << MaxStackSize;
	
	bOutSuccess = true;
	return true;
}

UScriptStruct* UInventoryStackItemType::GetItemDataType() const
{
	return FInventoryStackData::StaticStruct();
}

TSharedPtr<FInventoryItemDataBase> UInventoryStackItemType::CreateItemData() const
{
	return MakeShareable(new FInventoryStackData());
}

int32 UInventoryStackItemType::AddToStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);

	if (!StackData.IsValid())
	{
		return 0;
	}

	return FStackItemTypeImplementation::AddToStack(StackData->StackCount, Count, MaxStackSize);
}

int32 UInventoryStackItemType::RemoveFromStack(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);

	if (!StackData.IsValid())
	{
		return 0;
	}

	return FStackItemTypeImplementation::RemoveFromStack(StackData->StackCount, Count);
}

void UInventoryStackItemType::SetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData, const int32 Count) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);

	if (!StackData.IsValid())
	{
		return;
	}

	StackData->StackCount = Count;
}


int32 UInventoryStackItemType::GetStackCount(TWeakPtr<FInventoryItemDataBase> ItemData) const
{
	const TSharedPtr<FInventoryStackData> StackData = ConvertDataChecked<FInventoryStackData>(ItemData);

	if (!StackData.IsValid())
	{
		return 0;
	}

	return StackData->StackCount;
}




// FStackItemTypeImplementation

int32 FStackItemTypeImplementation::AddToStack(int32& Stack, const int32 Count, const int32 MaxStackSize)
{
	const int32 PreviousStackCount = Stack;
	Stack = FMath::Min(Stack + Count, MaxStackSize);
	return Stack - PreviousStackCount;
}

int32 FStackItemTypeImplementation::RemoveFromStack(int32& Stack, const int32 Count)
{
	const int32 PreviousStackCount = Stack;
	Stack = FMath::Max(Stack - Count, 0);
	return PreviousStackCount - Stack;
}

