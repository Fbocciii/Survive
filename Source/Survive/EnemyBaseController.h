// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyBaseController.generated.h"
/**
 *
 */
UCLASS()
class SURVIVE_API AEnemyBaseController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyBaseController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		UBehaviorTree* BehaviorTree = nullptr;

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		UAIPerceptionComponent* Perception = nullptr;*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		AActor* PatrolCenter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		class UAISenseConfig_Sight* SightConfig = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AISightRadius = 1500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AILoseSightRadius = AISightRadius + 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AISightAge = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AIFieldOfView = 60.0f;


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FName TargetKeyName = "Target";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FName CenterKeyName = "PatrolCenter";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FName LocationKeyName = "Location";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FName CurrentHealthKeyName = "CurrentHealth";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FName IsSuspiciousKeyName = "IsSuspicious";

	virtual void Possess(APawn* InPawn) override;
	

	/*UFUNCTION()
		virtual void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);*/
	UFUNCTION()
		virtual void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
};
