// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurviveCharacter.h"
#include "Damagable.h"
#include "LockOnSystem.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "MasterEnemy.generated.h"

UCLASS()
class SURVIVE_API AMasterEnemy : public ASurviveCharacter, public ILockOnSystem
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMasterEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//Enemy target variables

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		class APlayerCharacter* CharacterRef = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		bool ShowCross;

	//Enemy health Widget

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UEnemyHealth> wHealthBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		UEnemyHealth* EnemyHealthBar = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		class UWidgetComponent* HealthBarComponent = nullptr;



	virtual void UpdateHealthBar() override;
	virtual void DealDamage_Implementation(const float& Damage, const AActor* Sender, bool& Success) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Events")
		void Death();
	void Death_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Events")
		void GetPlayer();
	void GetPlayer_Implementation();

	//flip flop bool
	bool IsA = true;

	//Components
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USceneComponent* WarpLocation = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AActor* AIPatrolCenter = nullptr;
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UWidgetComponent* CrosshairWidgetA = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* LockOnSphere = nullptr;

	//Timeline variables
	UTimelineComponent* CrosshairTimeline = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timeline")
		class UCurveFloat* fCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
		FVector MinCrosshairScale = { 0.0f, 0.0f, 0.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
		FVector MaxCrosshairScale = { 0.5f, 0.5f, 0.5f };

	UPROPERTY()
		TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		bool CanBeTargeted;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		float PlayerToEnemyDistance = 0.0f;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Calculation")
		void DistanceToPlayer();
	void DistanceToPlayer_Implementation();


	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;




	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void LockOn(bool& Success);
	virtual void LockOn_Implementation(bool& Success) override;

	FTimerHandle DeathTimerHandle;

	FTimerHandle GetPlayerHandle;

	FTimerHandle DistanceToPlayerHandle;

	UFUNCTION()
		void OnTimerEnd();



	//Timeline functions
	UFUNCTION()
		void CrosshairTimelineFloatReturn(float value);

	UFUNCTION()
		void OnCrosshairTimelineFinished();
};
