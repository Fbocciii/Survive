// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interacts.h"
#include "InteractionMarker.generated.h"

UCLASS()
class SURVIVE_API AInteractionMarker : public AActor, public IInteracts
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor* Interactor, bool& Success);
	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;
};
