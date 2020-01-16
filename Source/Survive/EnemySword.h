// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sword.h"
#include "EnemySword.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVE_API AEnemySword : public ASword
{
	GENERATED_BODY()
	


public:
	virtual void UpdateCharacterRef() override; //Change variable to establish correct owning actor
};
