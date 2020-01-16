// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "ItemMaster.h"
#include "Class.h"
#include "ObjectMacros.h"
#include "InventoryComponent.generated.h"


UENUM(BlueprintType)
enum class EPins : uint8
{
	TruePin UMETA(DisplayName = "True"),
	FalsePin UMETA(DisplayName = "False")
};


USTRUCT(BlueprintType)
struct FSlotStructure
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FItemStructure ItemStructure;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Quantity = 0;
	
};

USTRUCT(BlueprintType)
struct FInventoryQuery
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AItemMaster> QueryItem;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 QueryAmount = 0;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FText InventoryName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int32 NumberOfSlots = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FVector2D Position;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSlotStructure> Inventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PrepareInventory();

	UFUNCTION(BlueprintCallable)
	void CreateStack(const FSlotStructure& ContentToAdd, bool& Success);

	UFUNCTION(BlueprintCallable)
	void AddToStack(const FSlotStructure& ContentToAdd, int32 SlotIndex, bool& Success);

	UFUNCTION(BlueprintCallable, Category = "Macros", Meta = (ExpandEnumAsExecs = "Pins"))
	void HasPartialStack(const FSlotStructure& SlotStructure, EPins& Pins, int32& StackIndex);

	UFUNCTION(BlueprintCallable)
	void AddToInventory(const FSlotStructure& ContentToAdd, bool& Success);

	UFUNCTION(BlueprintCallable)
	void InventoryQuery(const TArray<FInventoryQuery>& Queries, bool& IsInInventory, TArray<int32>& SlotIndices);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void ToggleInventory();
	//void ToggleInventory_Implementation();

};

