// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskLightAttack.h"
#include "SurviveCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskLightAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	
	ASurviveCharacter* Character = Cast<ASurviveCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character)
	{
		Character->LightAttackInput();
	}

	return EBTNodeResult::Type::Succeeded;
}
