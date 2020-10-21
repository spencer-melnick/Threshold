// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "InventoryArray.generated.h"



// Forward declarations

struct FInventoryArray;



/**
 * Handle to a specific item in an inventory item array, that allows for fast removal and handles marking the array
 * as dirty when needed. Functions like a weak reference in that it doesn't prevent destruction of the item. It works
 * by maintaining a pointer to the inventory array and a unique ID that can be referenced to find the item in the array.
 * Furthermore, the array validity is checked by maintaining a weak object pointer to the owning object of the array (so
 * that theoretically when the owner is invalidated or garbage collected, we know the array is also invalid)
 */
USTRUCT()
struct INVENTORYSYSTEM_API FInventoryArrayHandle
{
	GENERATED_BODY()

	friend struct FInventoryArray;

	
public:
	FInventoryArrayHandle()
		: ItemID(INDEX_NONE), ArrayOwner(nullptr), Array(nullptr)
	{};

	FInventoryArrayHandle(const FInventoryArrayHandle& Other)
		: ItemID(Other.ItemID), ArrayOwner(Other.ArrayOwner), Array(Other.Array)
	{};

	FInventoryArrayHandle& operator=(const FInventoryArrayHandle& Other)
	{
		ItemID = Other.ItemID;
		ArrayOwner = Other.ArrayOwner;
		Array = Other.Array;
		return *this;
	}

	
	// Accessors

	/**
	 * Returns true if the handle has an ID of INDEX_NONE, Array of nullptr, or the array owner is invalid
	 */
	bool IsNull() const { return ItemID == INDEX_NONE || Array == nullptr || !ArrayOwner.IsValid(); }

	/**
	 * Returns the owner of the inventory array with no checks
	 */
	TWeakObjectPtr<> GetOwner() const
	{
		return ArrayOwner;
	};

	/**
	 * Returns the referenced inventory array with no checks
	 */
	FInventoryArray* GetArray() const
	{
		return Array;
	}

	/**
	 * Returns the underlying inventory item's array index, or INDEX_NONE if the handle is invalid
	 */
	int32 GetIndex() const;

	/**
	 * Accesses the underlying inventory item - does not mark the state as dirty. The pointer is temporary and should not
	 * be used directly, as any resize, move, or deletion operations on the inventory array could invalidate this pointer
	 * @return The underlying item, or nullptr if the handle is invalid for any reason
	 */
	FInventoryItem* Get() const;


	FInventoryItem* operator->() const { return Get(); }


	// Array modifiers

	/**
	 * Notifies the inventory component that we modified this inventory item so it can update replication and UI as needed
	 */
	void MarkDirty() const;

	/**
	 * Deletes this item from the inventory component and notifies the array to update its state accordingly
	 */
	void Remove();

	/**
	 * Clears the state of this inventory item handle so it no longer refers to anything
	 */
	void Clear() { ItemID = INDEX_NONE; ArrayOwner = nullptr; Array = nullptr; }


protected:
	FInventoryArrayHandle(int32 ItemID, UObject* ArrayOwner, FInventoryArray* Array)
		: ItemID(ItemID), ArrayOwner(ArrayOwner), Array(Array)
	{};


private:
	int32 ItemID;
	TWeakObjectPtr<UObject> ArrayOwner;
	FInventoryArray* Array;
};



// Delegates

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryArrayItemChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE(FInventoryArrayChangedDelegate);



/**
* Struct that wraps an array of inventory items for use with a fast array serializer
*/
USTRUCT()
struct FInventoryArray : public FFastArraySerializer
{
	GENERATED_BODY()

	friend struct FInventoryArrayHandle;

public:

	FInventoryArray()
		: Owner(nullptr)
	{};
	

	FInventoryArray(UObject* Owner)
		: Owner(Owner)
	{};

	
	// Array serialization
	
	void PostSerialize(FArchive& Ar);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);


	// Array operations

	/**
	 * Emplaces an element in the array
	 * @return Handle to the new element
	 */
	template<typename ... ArgsType>
	FInventoryArrayHandle Emplace(ArgsType&&... Args)
	{
		// Create the new item and assign it an ID
		FInventoryItem& NewItem = Items.Emplace_GetRef(Forward<ArgsType>(Args)...);
		NewItem.UniqueID = GetNextID();
		IDtoIndex.Add(NewItem.UniqueID, Items.Num() - 1);

		// Update any state
		MarkItemDirty(NewItem);
		NotifyArrayChanged();

		return FInventoryArrayHandle(NewItem.UniqueID, Owner, this);
	}

	/**
	 * Removes an element based on its handle
	 */
	void Remove(FInventoryArrayHandle& ItemHandle);

	/**
	 * Removes all elements for which the predicate returns true
	 */
	template <class PredicateClass>
	int32 RemoveAll(const PredicateClass& Predicate)
	{
		const int32 RemovalCount = Items.RemoveAll(Predicate);

		if (RemovalCount > 0)
		{
			NotifyItemsDeleted();
		}

		return RemovalCount;
	}

	/**
	 * Empties the entire array
	 * @param Slack - Optional expected usage size after emptying
	 */
	void Empty(int32 Slack = 0);

	/**
	 * Finds all elements for which the predicate returns true
	 * @return Array of pointers to the elements in the array. Should only be used temporarily - any insertions or
	 * deletions could invalidate the pointers.
	 */
	template <class PredicateClass>
	TArray<FInventoryItem*> FindAllTemporary(const PredicateClass& Predicate)
	{
		TArray<FInventoryItem*> Result;

		for (FInventoryItem& Item : Items)
		{
			if (Predicate(Item))
			{
				Result.Add(&Item);
			}
		}

		return Result;
	}

	/**
	 * Finds all elements for which the predicate returns true
	 * @return Array of handles referencing the elements. Can be stored and referenced later
	 */
	template <class PredicateClass>
	TArray<FInventoryArrayHandle> FindAll(const PredicateClass& Predicate)
	{
		TArray<FInventoryArrayHandle> Result;

		for (FInventoryItem& Item : Items)
		{
			if (Predicate(Item))
			{
				Result.Add(FInventoryArrayHandle(Item.UniqueID, Owner, this));
			}
		}

		return Result;
	}

	/**
	 * Finds the first element for which the predicate returns true
	 * @return Pointer to the found element, or null. Should only be used temporarily - insertions or deletions
	 * could invalidate the pointer
	 */
	template <class PredicateClass>
	FInventoryItem* FindTemporary(const PredicateClass& Predicate) { return (Items.FindByPredicate(Predicate)); }

	/**
	 * Finds the first element for which the predicate returns true
	 * @return Handle to the found element. IsNull will be true if no element was found
	 */
	template <class PredicateClass>
	FInventoryArrayHandle Find(const PredicateClass& Predicate)
	{
		const FInventoryItem* Item = FindTemporary(Predicate);

		if (!Item)
		{
			return FInventoryArrayHandle();
		}

		return FInventoryArrayHandle(Item->UniqueID, Owner, this);
	}

	/**
	 * Access the underlying array
	 */
	const TArray<FInventoryItem>& GetArray() const { return Items; }

	/**
	 * Returns handles to all of the items in the underlying array
	 */
	TArray<FInventoryArrayHandle> GetArrayHandles();


	// Delegates

	/**
	 * Called when any item is modified
	 */
	FInventoryArrayChangedDelegate InventoryArrayChangedDelegate;
	

	
protected:

	// State changes

	/**
	 * Notifies any listeners that the array was updated
	 */
	void NotifyArrayChanged();

	/**
	 * Marks the array as dirty, rebuilds the ID map, and notifies any listeners
	 */
	void NotifyItemsDeleted();


	// Helper functions

	int32 GetNextID() { checkf(LastItemID != INDEX_NONE, TEXT("Inventory array ID exceeded limits - may God have mercy on our souls")); return LastItemID++; }
	
	/**
	* Builds a map of unique item IDs to inventory indices
	*/
	void RebuildIDMap();

	/**
	 * Look up an element's index by its unique ID - will rebuild the ID map if needed
	 * @return Element's array index or INDEX_NONE if it is not valid
	 */
	int32 LookupIndex(const int32 UniqueID);

	
private:

	UPROPERTY(NotReplicated)
	UObject* Owner;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FInventoryItem> Items;

	UPROPERTY(NotReplicated)
	TMap<int32, int32> IDtoIndex;

	UPROPERTY(NotReplicated)
	int32 LastItemID = 0;

	UPROPERTY(NotReplicated)
	bool bNeedsIDRebuild = false;

	UPROPERTY(NotReplicated)
	bool bReceivedChanges = false;
};



/**
* Enables fast network serialization of an inventory array
*/
template <>
struct TStructOpsTypeTraits<FInventoryArray> : public TStructOpsTypeTraitsBase2<FInventoryArray>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};
