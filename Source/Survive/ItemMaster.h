// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interacts.h"
#include "GameFramework/Actor.h"
#include "ItemMaster.generated.h"

class AItemMaster;

USTRUCT(BlueprintType)
struct FItemStructure
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UTexture2D* Thumbnail = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 MaxStackSize = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Durability = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AItemMaster> Class;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsConsumable = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsStackable = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsQuestItem = false;
};

UCLASS()
class SURVIVE_API AItemMaster : public AActor, public IInteracts
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Use")
		void UseItem(bool& Success);
	void UseItem_Implementation(bool& Success);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FItemStructure ItemStructure;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor* Interactor, bool& Success);
	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void PickUpItem();
	void PickUpItem_Implementation();
};
