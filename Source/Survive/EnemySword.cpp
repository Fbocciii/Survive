// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySword.h"
#include "MasterEnemy.h"

void AEnemySword::UpdateCharacterRef()
{
	if (GetParentActor())
	{
		CharacterRef = Cast<AMasterEnemy>(GetParentActor());
	}
}
