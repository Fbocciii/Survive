// Fill out your copyright notice in the Description page of Project Settings.

#include "LocationMarker.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Quest.h"
#include "QuestLog.h"
#include "Kismet/GameplayStatics.h"
#include "SurviveCharacter.h"

// Sets default values
ALocationMarker::ALocationMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereA = CreateDefaultSubobject<USphereComponent>(FName("Collider"));
	SphereA->SetupAttachment(RootComponent);
	if (SphereA)
	{
		SphereA->OnComponentBeginOverlap.AddDynamic(this, &ALocationMarker::Overlap);
	}
}

// Called when the game starts or when spawned
void ALocationMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALocationMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALocationMarker::Overlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASurviveCharacter* Player = Cast<ASurviveCharacter>(OtherActor);

	if (Player && Player->GetCapsuleComponent() == Cast<UCapsuleComponent>(OtherComp))
	{
		UQuestLog* Log = Cast<UQuestLog>(Player->GetComponentByClass(UQuestLog::StaticClass()));
		if (Log)
		{
			for (int32 i = 0; i < Log->QuestsA.Num(); i++)
			{
				Log->QuestsA[i]->LocationFound.Broadcast(this);
			}
		}
	}
}

