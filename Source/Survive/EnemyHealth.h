// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealth.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVE_API UEnemyHealth : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (BindWidget))
	class UProgressBar* Health = nullptr;

	void Init();
	void Update(float Percent);
};
