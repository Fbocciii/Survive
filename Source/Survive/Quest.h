// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LocationMarker.h"
#include "ItemMaster.h"
#include "MasterEnemy.h"
#include "Interacts.h"
#include "Quest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationReached, ALocationMarker*, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, AActor*, InteractionTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollectedItem, AItemMaster*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilled, AMasterEnemy*, Enemy);

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	Location,
	Interact,
	Collect,
	Kill
};

USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EObjectiveType Type;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsComplete;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Number;
};


UCLASS()
class SURVIVE_API AQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		class UInventoryComponent* PlayerInventoryRef = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Delegate handles
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "EventDispatchers")
		FOnLocationReached LocationFound;
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "EventDispatchers")
		FOnInteracted ActorInteracted;
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "EventDispatchers")
		FOnCollectedItem ItemCollected;
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "EventDispatchers")
		FOnEnemyKilled EnemyKilled;

	//Events to bind to delegates
	UFUNCTION(BlueprintCallable, Category = "Events")
		void CheckLocation(ALocationMarker* Location);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void CheckInteraction(AActor* InteractionTarget);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void CheckCollection(AItemMaster* Item);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void CheckKill(AMasterEnemy* Enemy);

	//Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		FText NameA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		FText DescriptionA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		FText TurnInTextA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		bool IsStoryQuestA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		bool IsCompleteA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		bool IsInProgress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		int32 CurrentKillCountA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		float XPRewardA;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		AQuest* PreRequisiteA = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
		TArray<FObjectiveData> ObjectivesA;
};
