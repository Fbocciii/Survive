// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthStamina.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVE_API UHealthStamina : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (BindWidget))
	class UProgressBar* Health = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (BindWidget))
	UProgressBar* Stamina = nullptr;


	void Init();
	void UpdateHealth(float Percent);
	void UpdateStamina(float Percent);
};
