// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskFindLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskFindLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	FVector Location;
	UNavigationSystemBase* NavSystem = GetWorld()->GetNavigationSystem();	
	
	FVector Center;
	Center = OwnerComp.GetBlackboardComponent()->GetValueAsVector(PatrolCenterKey.SelectedKeyName);

	bool FoundPoint = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(),
																			  Center,
																			  Location,
																			  Radius);
	if (!FoundPoint)
	{
		Location = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(LocationKey.SelectedKeyName, Location);


	
	return EBTNodeResult::Type::Succeeded;
}
