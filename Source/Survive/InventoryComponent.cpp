 // Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::PrepareInventory()
{
	if (Inventory.Num() != NumberOfSlots)
	{
		Inventory.SetNum(NumberOfSlots);
	}

	//Mainly sanity checking, ensure array is valid before use
	FItemStructure cur;
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		cur = Inventory[i].ItemStructure;
		cur.Class;
		if (cur.Class != nullptr)
		{
			Inventory[i].ItemStructure = cur.Class.GetDefaultObject()->ItemStructure;
		}
	}
}

void UInventoryComponent::CreateStack(const FSlotStructure & ContentToAdd, bool & Success)
{

	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Quantity <= 0)
		{
			Success = true;
			Inventory[i] = ContentToAdd;
			return;
		}
	}
	Success = false;
}

void UInventoryComponent::AddToStack(const FSlotStructure & ContentToAdd, int32 SlotIndex, bool & Success)
{
	
	if(Inventory[SlotIndex].Quantity + ContentToAdd.Quantity > Inventory[SlotIndex].ItemStructure.MaxStackSize)
	{
		int32 CurrentStackQuantity = Inventory[SlotIndex].Quantity;
		Inventory[SlotIndex].Quantity = Inventory[SlotIndex].ItemStructure.MaxStackSize;

		int32 Overflow = CurrentStackQuantity + ContentToAdd.Quantity - Inventory[SlotIndex].ItemStructure.MaxStackSize;

		FSlotStructure SlotStructure;
		SlotStructure.ItemStructure = ContentToAdd.ItemStructure;
		SlotStructure.Quantity = Overflow;
		AddToInventory(SlotStructure, Success);
	}
	else
	{
		Inventory[SlotIndex].Quantity += ContentToAdd.Quantity;
	}

	Success = true;
}

void UInventoryComponent::HasPartialStack(const FSlotStructure & SlotStructure, EPins& Pins, int32 & StackIndex)
{
	StackIndex = -1;

	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Quantity < Inventory[i].ItemStructure.MaxStackSize && 
			Inventory[i].ItemStructure.Class == SlotStructure.ItemStructure.Class)
		{
			StackIndex = i;
			Pins = EPins::TruePin;
			return;
		}
	}

	Pins = EPins::FalsePin;
}

void UInventoryComponent::AddToInventory(const FSlotStructure & ContentToAdd, bool & Success)
{
	if (ContentToAdd.ItemStructure.IsStackable)
	{
		EPins Result;
		int32 StackIndex;
		HasPartialStack(ContentToAdd, Result, StackIndex);
		switch (Result)
		{
			case EPins::TruePin:
				AddToStack(ContentToAdd, StackIndex, Success);
				return;
				break;
			case EPins::FalsePin:
				CreateStack(ContentToAdd, Success);
				return;
				break;
			default:
				break;
		}
	}
	else
	{
		CreateStack(ContentToAdd, Success);
	}
}

void UInventoryComponent::InventoryQuery(const TArray<FInventoryQuery>& Queries, bool & IsInInventory, TArray<int32>& SlotIndices)
{
	bool Success;
	int32 CurrentAmount = 0;
	for (int32 i = 0; i < Queries.Num(); i++)
	{
		Success = false;
		CurrentAmount = 0;
		for (int32 j = 0; j < Inventory.Num(); j++)
		{
			if (Queries[i].QueryItem == Inventory[j].ItemStructure.Class)
			{
				CurrentAmount += Inventory[j].Quantity;
				SlotIndices.Push(j);
				if (CurrentAmount >= Queries[i].QueryAmount)
				{
					Success = true;
				}
			}
		}
		if (!Success)
		{
			break;
		}
	}
	IsInInventory = Success;
}

//void UInventoryComponent::ToggleInventory_Implementation()
//{
//	ToggleInventory();
//	UE_LOG(LogTemp, Warning, TEXT("Inventory UI"));
//}

