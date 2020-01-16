// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthStamina.h"
#include "UMG/Public/Components/ProgressBar.h"

void UHealthStamina::Init()
{
}

void UHealthStamina::UpdateHealth(float Percent)
{
	if (Health)
	{
		Health->SetPercent(Percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Health progress bar is nullptr"));
	}
}

void UHealthStamina::UpdateStamina(float Percent)
{
	if (Stamina)
	{
		Stamina->SetPercent(Percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stamina progress bar is nullptr"));
	}
}
