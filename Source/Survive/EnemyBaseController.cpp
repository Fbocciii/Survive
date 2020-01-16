// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBaseController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MasterEnemy.h"
#include "GameplayDebuggerCategory.h"

AEnemyBaseController::AEnemyBaseController() : AAIController()
{

	/*PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("PerceptionComponent"));

	SenseConfig = NewObject<UAISenseConfig>();
	PerceptionComponent->ConfigureSense(*SenseConfig);*/


	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("PerceptionComponent"));
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBaseController::OnTargetPerceptionUpdate);
	SetPerceptionComponent(*PerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	PerceptionComponent->ConfigureSense(*SightConfig);

}

void AEnemyBaseController::BeginPlay()
{
	Super::BeginPlay();


}

void AEnemyBaseController::Possess(APawn * PossessedPawn)
{
	Super::Possess(PossessedPawn);

	//GetPawn()->GetMovementComponent()->set
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed *= .5f;

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		AMasterEnemy* AI = Cast<AMasterEnemy>(GetPawn());

		if (AI)
		{
			PatrolCenter = AI->AIPatrolCenter;
		}

		if (PatrolCenter)
		{

			GetBlackboardComponent()->SetValueAsVector(CenterKeyName, PatrolCenter->GetActorLocation());
		}
		else
		{
			GetBlackboardComponent()->SetValueAsVector(CenterKeyName, GetPawn()->GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No behavior tree assigned to AI!"));
	}


}

void AEnemyBaseController::OnTargetPerceptionUpdate(AActor * Actor, FAIStimulus Stimulus)
{
	TSubclassOf<ACharacter> CharacterClass;
	if ((CharacterClass = Actor->GetClass()) && (Actor->GetClass() != GetPawn()->GetClass()))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsObject(TargetKeyName, Actor);
			GetBlackboardComponent()->SetValueAsBool(IsSuspiciousKeyName, false);
		}
		else
		{
			GetBlackboardComponent()->ClearValue(TargetKeyName);
			GetBlackboardComponent()->SetValueAsBool(IsSuspiciousKeyName, true);
			GetBlackboardComponent()->SetValueAsVector(LocationKeyName, Actor->GetActorLocation());
		}
		

		UE_LOG(LogTemp, Warning, TEXT("Target set as: %s!"), *Actor->GetName());

	}
}



//void AEnemyBaseController::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
//{
//	TSubclassOf<ACharacter> CharacterClass;
//
//	for (AActor* Actor : UpdatedActors)
//	{
//		//TODO: Check if actor is of specific class, if so then set as target and break
//		
//		if ((CharacterClass = Actor->GetClass()) && (Actor->GetClass() != GetPawn()->GetClass()))
//		{
//			if (PerceptionComponent->GetActorInfo(Actor).)
//			{
//
//			}
//			GetBlackboardComponent()->SetValueAsObject(TargetKeyName, Actor);
//
//			UE_LOG(LogTemp, Warning, TEXT("Target set as: %s!"), *Actor->GetName());
//			break;
//		}
//	}
//}
