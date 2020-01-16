// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWindow.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/GridPanel.h"
#include "InventoryComponent.h"

void UInventoryWindow::CloseMenu()
{
	if (Inventory)
	{
		//Inventory->ToggleInventory();
	}
}

void UInventoryWindow::NativeConstruct()
{
	ButtonClose->OnClicked.AddDynamic(this, &UInventoryWindow::CloseMenu);

}

void UInventoryWindow::Init(UInventoryComponent * Inventory)
{
	this->Inventory = Inventory;

	if (Inventory)
	{
		InventoryTitle = Inventory->InventoryName;

		for (size_t i = 0; i < Inventory->NumberOfSlots; i++)
		{
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory window's Inventory reference is null!"));
	}
}
