// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurviveCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class SURVIVE_API APlayerCharacter : public ASurviveCharacter
{
	GENERATED_BODY()


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:

	APlayerCharacter();

	virtual void BeginPlay() override;



	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UHealthStamina> wHealthStaminaBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		UHealthStamina* HealthStaminaWidget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 CurrentStamina = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 MaxStamina = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 StaminaRegenerateValue = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SprintSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float NormalSpeed = 600.0f;

	virtual bool CheckStamina(int32 Threshold) override;

	void StaminaDecrease(int32 DecreasedStamina) override;

	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Helper")
		class UInventoryComponent* GetInventory() const;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Helper")
		void UpdateObjectivePoly();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Helper")
		class UQuestLog* GetQuestLog() const;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Helper")
		void AddQuestUI(class AQuest* Quest) const;

	
		virtual void UpdateHealthBar() override;
	UFUNCTION(BlueprintCallable, Category = "Base")
		void UpdateStaminaBar();

	UFUNCTION(BlueprintNativeEvent, Category = "Sprint")
		void SprintStart();
	virtual void SprintStart_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Sprint")
		void SprintEnd();
	virtual void SprintEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Sprint")
		void StartStopRegeneratingStamina();
	virtual void StartStopRegeneratingStamina_Implementation();
	/*UFUNCTION(BlueprintCallable, "Base")
		void StaminaDecrease(int32 DecreasedStamina);*/

	FTimerHandle SprintStaminaDecreaseHandle;
	FTimerHandle RegenerateStaminaHandle;


	UFUNCTION(BlueprintCallable, Category = "Sprint")
		void SprintStaminaDecrease();
	UFUNCTION(BlueprintCallable, Category = "Base")
		void RegenerateStamina();
	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
		UStaticMeshComponent* Cone = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
		UCapsuleComponent* InteractionField = nullptr;

	UFUNCTION()
		void BeginConeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void EndConeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void BeginInteractionFieldOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void EndInteractionFieldOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<class UWidgetComponent> InteractMessageClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		UWidgetComponent* InteractMessageUI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		float ClosestTargetDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		float MaximumDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		float ClosestTargetRotation = 0.0f;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void LockOnEvent();
	void LockOnEvent_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void LockOnDone();
	void LockOnDone_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void SetLockOnToTarget();
	void SetLockOnToTarget_Implementation();

	FTimerHandle ToggleLockOnHandle;
	UFUNCTION(BlueprintCallable, Category = "LockOn")
		void ToggleLockOn();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
		void NewControlRotation(FRotator& Rotator, AController*& NewController);

	UFUNCTION(BlueprintCallable, Category = "LockOn")
		void MovementLockOn();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void SwitchToLeft();
	void SwitchToLeft_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void SwitchToRight();
	void SwitchToRight_Implementation();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
		void SwitchDone();

	virtual void DealDamage_Implementation(const float& Damage, const AActor* Sender, bool& Success) override;
};
