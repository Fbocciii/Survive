// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interacts.h"
#include "InventoryComponent.h"
#include "ContainerMaster.generated.h"


UCLASS()
class SURVIVE_API AContainerMaster : public AActor, public IInteracts
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AContainerMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
		TArray<FSlotStructure> ItemsA;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor* Interactor, bool& Success);
	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;
};
