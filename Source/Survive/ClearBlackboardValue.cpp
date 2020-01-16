// Fill out your copyright notice in the Description page of Project Settings.

#include "ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

EBTNodeResult::Type UClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	OwnerComp.GetBlackboardComponent()->ClearValue(KeyToClear.SelectedKeyName);
	/*if (KeyToClear.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(KeyToClear.SelectedKeyName, false);
	}
	else
	{
		
	}*/
	return EBTNodeResult::Type::Succeeded;
}
