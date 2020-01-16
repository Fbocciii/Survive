// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemMaster.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "InventoryComponent.h"
#include "QuestLog.h"
#include "Quest.h"
#include "PlayerCharacter.h"

// Sets default values
AItemMaster::AItemMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AItemMaster::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AItemMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AItemMaster::UseItem_Implementation(bool & Success)
{
	Success = true;
}

void AItemMaster::Interact_Implementation(const AActor* Interactor, bool& Success)
{
	if (ItemStructure.IsQuestItem)
	{
		const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
		//GetQuestLog

		if (Player)
		{
			UQuestLog* Log = Player->GetQuestLog();//Cast<UQuestLog>(Player->GetComponentByClass(UQuestLog::StaticClass()));
			if (Log)
			{
				for (const AQuest* Quest : Log->QuestsA)
				{
					for (const FObjectiveData Objective : Quest->ObjectivesA)
					{
						if (Objective.Target->GetClass() == this->GetClass())
						{
							PickUpItem();
						}
					}
				}
			}
		}
	}
	else
	{
		PickUpItem();
		UE_LOG(LogTemp, Warning, TEXT("Interact Item"));
	}


}

void AItemMaster::PickUpItem_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("C++ Pick Up"));
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (Player && (Player->GetInventory()))
	{
		UInventoryComponent* PlayerInventory = Player->GetInventory();//Cast<UInventoryComponent>(Player->GetComponentByClass(UInventoryComponent::StaticClass()));
		bool Success = false;
		FSlotStructure Slot = { ItemStructure, 1 };

		PlayerInventory->AddToInventory(Slot, Success);
		if (Success)
		{
			Destroy();
			UQuestLog* Log = Player->GetQuestLog();//Cast<UQuestLog>(Player->GetComponentByClass(UQuestLog::StaticClass()));
			if (Log)
			{
				for (int32 i = 0; i < Log->QuestsA.Num(); i++)
				{
					Log->QuestsA[i]->ItemCollected.Broadcast(this);
					UE_LOG(LogTemp, Warning, TEXT("Brodcasting item pick up Event!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Player quest log return null!"));
			} 

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player inventory return null!"));
		}
	}

}

