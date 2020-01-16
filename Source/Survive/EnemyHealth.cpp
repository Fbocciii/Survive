// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyHealth.h"
#include "UMG/Public/Components/ProgressBar.h"

void UEnemyHealth::Init()
{
	
}

void UEnemyHealth::Update(float Percent)
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
