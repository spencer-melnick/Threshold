// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "InventorySubsystem.generated.h"


// Forward declarations

class UInventoryTableItem;


/**
 * Custom map function that allows hashing FNames via their string equivalent, since normal FName hashing does
 * not match across the network or different platforms.
 */
template <typename ValueType>
struct TItemSubsystemKeyFuncs : public BaseKeyFuncs<TPair<FString, ValueType>, FString>
{
private:
	typedef BaseKeyFuncs<TPair<FString, ValueType>, FString> Super;

public:
	typedef typename Super::ElementInitType ElementInitType;
	typedef typename Super::KeyInitType KeyInitType;

	static KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element.Key;
	}

	static bool Matches(KeyInitType A, KeyInitType B)
	{
		return A == B;
	}

	static uint32 GetKeyHash(KeyInitType Key)
	{
		// Convert the string to a UTF8 string so that endianness doesn't matter
		const FTCHARToUTF8 KeyUtf8(*Key);
		return CityHash32(KeyUtf8.Get(), KeyUtf8.Length());
	}
};


/**
 * Subsystem that loads inventory objects created via other assets, such as from a data table. Scans the paths
 * on game start and produces stably named objects that support network replication.
 */
UCLASS(Config=Game)
class UInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


	// Accessors

	UInventoryTableItem* GetItemByName(FString ItemName)
	{
		TStrongObjectPtr<UInventoryTableItem>* FindResult = InventoryTableItems.Find(ItemName);
		if (!FindResult)
		{
			return nullptr;
		}

		return FindResult->Get();
	}

	UInventoryTableItem* GetItemByHash(uint32 Hash)
	{
		TStrongObjectPtr<UInventoryTableItem>* FindResult = InventoryTableItems.FindByHash(Hash, );
	}


protected:
	// Helper functions

	void LoadTableItems();
	

private:
	UPROPERTY(Config)
	TArray<FString> InventoryTablePaths;

	TMap<FString, TStrongObjectPtr<UInventoryTableItem>, FDefaultSetAllocator, TItemSubsystemKeyFuncs<TStrongObjectPtr<UInventoryTableItem>>> InventoryTableItems;
};
