// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionMarker.h"
#include "QuestLog.h"
#include "Quest.h"
#include "PlayerCharacter.h"

// Sets default values
AInteractionMarker::AInteractionMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractionMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractionMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractionMarker::Interact_Implementation(const AActor * Interactor, bool & Success)
{
	const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);

	if (Player)
	{
		UQuestLog* Log = Player->GetQuestLog();//Cast<UQuestLog>(Player->GetComponentByClass(UQuestLog::StaticClass()));
		if (!Log)
		{
			return;
		}
		for (int32 i = 0; i < Log->QuestsA.Num(); i++)
		{
			Log->QuestsA[i]->ActorInteracted.Broadcast(this);
			UE_LOG(LogTemp, Warning, TEXT("Brodcasting Interacting Event!"));
		}
	}
}

