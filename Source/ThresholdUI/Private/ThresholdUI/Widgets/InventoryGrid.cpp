// Copyright (c) 2020 Spencer Melnick

// ReSharper disable CppExpressionWithoutSideEffects

#include "ThresholdUI/Widgets/InventoryGrid.h"
#include "ThresholdUI/Widgets/InventoryBlock.h"
#include "ThresholdUI/Blueprint/SelectionFunctionLibrary.h"
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



// Navigation

FIntPoint UInventoryGrid::GetAdjacentCell(const FIntPoint Cell, const ESelectionDirection Direction) const
{
	FIntPoint AdjacentCell = Cell + USelectionFunctionLibrary::GetUnitCoordinateFromDirection(Direction);
	
	if (!IsCellValid(AdjacentCell))
	{
		return FIntPoint(-1, -1);
	}

	return AdjacentCell;
}

UInventoryBlock* UInventoryGrid::GetBlockFromCell(FIntPoint Cell) const
{
	if (!IsCellValid(Cell))
	{
		return nullptr;
	}

	const int32 BlockIndex = Cell.Y * GridSize.X + Cell.X;

	if (BlockIndex < 0 || BlockIndex >= SubBlocks.Num())
	{
		UE_LOG(LogThresholdUI, Warning, TEXT("UInventoryGrid::GetBlockFromCell failed on %s - Cell {%d %d} index %d "
			"is out of range of grid size {%d, %d} and block count %d"),
			*GetNameSafe(this), Cell.X, Cell.Y, BlockIndex, GridSize.X, GridSize.Y, SubBlocks.Num())
		return nullptr;
	}

	return SubBlocks[BlockIndex];
}

void UInventoryGrid::SetSelectedCell(FIntPoint NewCell)
{
	if (!IsCellValid(NewCell))
	{
		return;
	}

	SelectedCell = NewCell;
}

void UInventoryGrid::SetDisplayBlock(UInventoryBlock* SelectedBlock)
{
	if (!SelectedBlock || SelectedBlock->GetParentGrid() != this || !IsCellValid(SelectedBlock->GetGridCell()))
	{
		return;
	}

	SetSelectedCell(SelectedBlock->GetGridCell());
	InventoryGridSelectedDelegate.ExecuteIfBound(SelectedBlock->GetItemHandle());
}


bool UInventoryGrid::IsCellValid(FIntPoint Cell) const
{
	if (Cell.X < 0 || Cell.X >= GridSize.X || Cell.Y < 0 || Cell.Y >= GridSize.Y)
	{
		return false;
	}

	return true;
}



// Selectable widget overrides

FSelectableWidgetReference UInventoryGrid::TrySelect(const ESelectionDirection FromSelectionDirection)
{
	// Find the last selected cell
	FIntPoint DefaultCell = SelectedCell;

	// Pick the cell closest to the direction we were selected from
	switch (FromSelectionDirection)
	{
		case ESelectionDirection::Up:
			DefaultCell.Y = 0;
			break;
		
		case ESelectionDirection::Down:
			DefaultCell.Y = GridSize.Y - 1;
			break;

		case ESelectionDirection::Left:
			DefaultCell.X = 0;
			break;

		case ESelectionDirection::Right:
			DefaultCell.X = GridSize.X - 1;
			break;

		default:
			break;
	}

	UInventoryBlock* DefaultBlock = GetBlockFromCell(DefaultCell);

	if (!DefaultBlock)
	{
		return nullptr;
	}

	return DefaultBlock->TrySelect(FromSelectionDirection);
}

void UInventoryGrid::InitializeSelection(TScriptInterface<ISelectionController> Controller)
{
	SelectionController = Controller;

	for (UInventoryBlock* InventoryBlock : SubBlocks)
	{
		// Try to initialize all of the sub block selection controllers if there are any
		InventoryBlock->InitializeSelection(Controller);
	}
}



// Helper functions

void UInventoryGrid::ConstructSubBlocks()
{
	// Clamp grid size to non-negative values
	GridSize = FIntPoint(FMath::Max(0, GridSize.X), FMath::Max(0, GridSize.Y));
	if (PreviousGridSize == GridSize)
	{
		// Skip
		return;
	}

	
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

	// Track the new grid size
	PreviousGridSize = GridSize;

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

			// Assign references to this and parent controllers
			InventoryBlock->SetParentGrid(this, FIntPoint(Row, Column));
			InventoryBlock->InitializeSelection(SelectionController);

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

