// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryGrid.h"
#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "ThresholdUI.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryOwner.h"


// UInventoryGrid

UInventoryGrid::UInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryBlockClass = UInventoryBlock::StaticClass();
	GridSize = FIntPoint(4, 3);
}



// User widget overrides

void UInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Reconstruct();
}


void UInventoryGrid::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	Reconstruct();
}



// Inventory controls

void UInventoryGrid::AssignInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	if (InventoryComponent == InInventoryComponent)
	{
		// Skip if nothing changes
		return;
	}

	if (InventoryComponent != nullptr)
	{
		// Clear update from previous inventory component delegate
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventoryGrid::UpdateDisplay);
	}

	// Assign properties to new values
	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &UInventoryGrid::UpdateDisplay);
	}
}


void UInventoryGrid::UpdateDisplay()
{
	if (!InventoryComponent)
	{
		for (UInventoryBlock* InventoryBlock : SubBlocks)
		{
			InventoryBlock->ClearDisplay();
		}

		return;
	}

	TArray<FInventoryArrayHandle> ItemHandles = InventoryComponent->GetArrayHandles();
	int32 HandleIndex = 0;
	
	for (UInventoryBlock* InventoryBlock : SubBlocks)
	{
		if (HandleIndex >= ItemHandles.Num())
		{
			InventoryBlock->ClearDisplay();
		}
		else
		{
			InventoryBlock->DisplayItem(ItemHandles[HandleIndex++]);
		}
	}
}



// Initialization

void UInventoryGrid::OnPlayerStateInitialized()
{
	IInventoryOwner* InventoryOwner = Cast<IInventoryOwner>(GetOwningPlayerState());
	if (InventoryOwner)
	{
		AssignInventoryComponent(InventoryOwner->GetInventoryComponent());
	}

	Reconstruct();
}




// Helper functions

void UInventoryGrid::ConstructSubBlocks()
{
	#if WITH_EDITOR
		// In the editor we may not have a grid panel yet
		if (!GridPanel)
		{
			return;
		}
	#endif

	
	if (!InventoryBlockClass)
	{
		UE_LOG(LogThresholdUI, Error, TEXT("UInventoryGrid::ConstructSubBlocks() failed on %s - invalid inventory block class"),
			*GetNameSafe(this))
		return;
	}

	for (UInventoryBlock* InventoryBlock : SubBlocks)
	{
		// Destroy all existing sub blocks
		InventoryBlock->RemoveFromParent();
	}

	if (GridSize.X < 0 || GridSize.Y < 0)
	{
		UE_LOG(LogThresholdUI, Warning, TEXT("UInventoryGrid::ConstructSubBlocks() halted on %s - invalid grid size of %d, %d"),
			*GetNameSafe(this), GridSize.X, GridSize.Y)
		SubBlocks.Empty();
		return;
	}

	// Clear all references to old widgets and reserve size for the new widgets
	const int32 NewSubBlockCount = GridSize.X * GridSize.Y;
	SubBlocks.Empty(NewSubBlockCount);
	GridSlots.Empty(NewSubBlockCount);

	for (int32 Row = 0; Row < GridSize.Y; Row++)
	{
		for (int32 Column = 0; Column < GridSize.X; Column++)
		{
			// Construct a new inventory block widget and track it
			UInventoryBlock* InventoryBlock = CreateWidget<UInventoryBlock>(this, InventoryBlockClass);
			SubBlocks.Add(InventoryBlock);

			// Add the block to the widget and track the slot
			UUniformGridSlot* GridSlot = GridPanel->AddChildToUniformGrid(InventoryBlock, Row, Column);
			GridSlot->SetHorizontalAlignment(HorizontalAlignment);
			GridSlot->SetVerticalAlignment(VerticalAlignment);
			GridSlots.Add(GridSlot);
		}
	}
}

void UInventoryGrid::Reconstruct()
{
	ConstructSubBlocks();
	UpdateDisplay();
}

