// Copyright (c) 2020 Spencer Melnick

#include "ThresholdUI/Widgets/InventoryGrid.h"
#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "ThresholdUI.h"
#include "Components/UniformGridPanel.h"


// UInventoryGrid

UInventoryGrid::UInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryBlockClass = UInventoryBlock::StaticClass();
	StartingInventoryIndex = 0;
	GridSize = FIntPoint(4, 3);
}



// User widget overrides

void UInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();

	Reconstruct();
}


void UInventoryGrid::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	Reconstruct();
}



// Inventory controls

void UInventoryGrid::AssignInventoryComponent(UInventoryComponent* InventoryComponent, int32 InStartingInventoryIndex)
{
	if (ParentInventory == InventoryComponent && StartingInventoryIndex == InStartingInventoryIndex)
	{
		// Skip if nothing changes
		return;
	}

	// Assign properties to new values
	ParentInventory = InventoryComponent;
	StartingInventoryIndex = InStartingInventoryIndex;

	BindSubBlocks();
}


void UInventoryGrid::UpdateDisplay()
{
	for (UInventoryBlock* InventoryBlock : SubBlocks)
	{
		InventoryBlock->UpdateDisplay();
	}
}





// Helper functions

void UInventoryGrid::ConstructSubBlocks()
{
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

	for (int32 Row = 0; Row < GridSize.Y; Row++)
	{
		for (int32 Column = 0; Column < GridSize.X; Column++)
		{
			// Construct a new inventory block widget, add the new widget to the grid and track it
			UInventoryBlock* InventoryBlock = CreateWidget<UInventoryBlock>(this, InventoryBlockClass);
			GridPanel->AddChildToUniformGrid(InventoryBlock, Row, Column);
			SubBlocks.Add(InventoryBlock);
		}
	}
}

void UInventoryGrid::BindSubBlocks()
{
	int32 CurrentInventoryIndex = StartingInventoryIndex;
	for (UInventoryBlock* InventoryBlock : SubBlocks)
	{
		// Bind each inventory block to a specific inventory index
		InventoryBlock->AssignToInventory(ParentInventory, CurrentInventoryIndex++);
	}

	UpdateDisplay();
}

void UInventoryGrid::Reconstruct()
{
	ConstructSubBlocks();
	BindSubBlocks();
	UpdateDisplay();
}

