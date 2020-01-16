// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWindow.generated.h"

/**
 *
 */
UCLASS()
class SURVIVE_API UInventoryWindow : public UUserWidget
{
	GENERATED_BODY()


		

protected:
	UFUNCTION()
		void CloseMenu();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
		FText InventoryTitle;
public:

	virtual void NativeConstruct() override;

	virtual void Init(class UInventoryComponent* Inventory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (BindWidget))
		UInventoryComponent* Inventory = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		FText _InventoryTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (BindWidget))
		class UButton* ButtonClose = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (BindWidget))
		class UGridPanel* InventoryGrid = nullptr;
};
