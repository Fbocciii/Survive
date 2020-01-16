// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "BTTaskEQSQuery.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVE_API UBTTaskEQSQuery : public UBTTask_RunEQSQuery
{
	GENERATED_BODY()
	

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};
