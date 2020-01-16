// Fill out your copyright notice in the Description page of Project Settings.

#include "Quest.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "InventoryComponent.h"

// Sets default values
AQuest::AQuest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
}

// Called when the game starts or when spawned
void AQuest::BeginPlay()
{
	Super::BeginPlay();
	
	ASurviveCharacter* Player = Cast<ASurviveCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player)
	{
		PlayerInventoryRef = Cast<UInventoryComponent>(Player->GetComponentByClass(UInventoryComponent::StaticClass()));
	}


	LocationFound.AddDynamic(this, &AQuest::CheckLocation);
	ActorInteracted.AddDynamic(this, &AQuest::CheckInteraction);
	ItemCollected.AddDynamic(this, &AQuest::CheckCollection);
	EnemyKilled.AddDynamic(this, &AQuest::CheckKill);
}

// Called every frame
void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuest::CheckLocation(ALocationMarker * Location)
{
	int32 ObjectiveNumber = 0;
	bool UpdateUI = false;
	for (int32 i = 0; i < ObjectivesA.Num(); i++)
	{
		ObjectiveNumber = i;
		if (!ObjectivesA[i].IsComplete && ObjectivesA[i].Target == Location)
		{
			ObjectivesA[i].IsComplete = true;
			UpdateUI = true;
			UE_LOG(LogTemp, Warning, TEXT("Location Found!"));
		}
	}
	if (UpdateUI)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			//TODO
			//Get player HUD and update objectives UI
			Player->UpdateObjectivePoly();
		}
	}
}

void AQuest::CheckInteraction(AActor * InteractionTarget)
{
	int32 ObjectiveNumber = 0;
	bool UpdateUI = false;
	for (int32 i = 0; i < ObjectivesA.Num(); i++)
	{
		ObjectiveNumber = i;
		if (!ObjectivesA[i].IsComplete && ObjectivesA[i].Target == InteractionTarget)
		{
			ObjectivesA[i].IsComplete = true;
			UpdateUI = true;
			UE_LOG(LogTemp, Warning, TEXT("Interaction Quest Complete!"));
		}
	}
	if (UpdateUI)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			//TODO
			//Get player HUD and update objectives UI
			Player->UpdateObjectivePoly();
		}
	}
}

void AQuest::CheckCollection(AItemMaster * Item)
{
	int32 ObjectiveNumber = 0;
	bool UpdateUI = false;
	for (int32 i = 0; i < ObjectivesA.Num(); i++)
	{
		ObjectiveNumber = i;
		if (Item->GetClass() == ObjectivesA[i].Target->GetClass())
		{
			TArray<FInventoryQuery> Queries = { {Item->GetClass(), ObjectivesA[i].Number} };
			bool HasItems = false;
			TArray<int32> Slots;
			PlayerInventoryRef->InventoryQuery(Queries, HasItems, Slots);

			if (HasItems)
			{
				ObjectivesA[i].IsComplete = true;
				UpdateUI = true; 
				UE_LOG(LogTemp, Warning, TEXT("Collection Quest Complete!"));
			}
		}
	}
	if (UpdateUI)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			//TODO
			//Get player HUD and update objectives UI
			Player->UpdateObjectivePoly();
		}
	}
}

void AQuest::CheckKill(AMasterEnemy * Enemy)
{
	int32 ObjectiveNumber = 0;
	bool UpdateUI = false;
	for (int32 i = 0; i < ObjectivesA.Num(); i++)
	{
		ObjectiveNumber = i;
		if (Enemy->GetClass() == ObjectivesA[i].Target->GetClass())
		{
			++CurrentKillCountA;
			if (CurrentKillCountA >= ObjectivesA[i].Number)
			{
				ObjectivesA[i].IsComplete = true;
				UpdateUI = true;
				UE_LOG(LogTemp, Warning, TEXT("Kill Quest Complete!"));
			}
			
		}
	}
	if (UpdateUI)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			//TODO
			//Get player HUD and update objectives UI
			Player->UpdateObjectivePoly();
		}
	}
}

