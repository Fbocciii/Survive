// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interacts.h"
#include "Damagable.h"
#include "SurviveCharacter.generated.h"

UCLASS(config=Game)
class ASurviveCharacter : public ACharacter, public IInteracts, public IDamagable
{
	GENERATED_BODY()

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ASurviveCharacter();

	virtual void BeginPlay() override;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		class UInventoryComponent* InventoryComponent = nullptr;*/
	

	virtual bool CheckStamina(int32 Threshold) { return true; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor* Interactor, bool& Success);
	virtual void Interact_Implementation(const AActor* Interactor, bool& Success) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 CurrentHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 MaxHealth = 150;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool PlayingAnim = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool IsWeaponEquipped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool IsWeaponSet = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CannotAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool LightAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool HeavyAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool IsChainAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool LastAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools", DisplayName = "Left/Right??")
		bool LeftRight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool TargetLocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool FoundNewTarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool IsInWarp = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CanWarp = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CanMove = false;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		int32 ChainLightAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		int32 ChainHeavyAttack = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class AWeaponMaster* EquippedWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<AWeaponMaster> AttachedWeapon;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
		class AMasterEnemy* NearestTarget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
		AMasterEnemy* EnemyElement = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
		AMasterEnemy* NewTarget = nullptr;


	UFUNCTION(BlueprintCallable, Category = "Base")
		virtual void StaminaDecrease(int32 DecreasedStamina) { return; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
		void PickUpEvent();
	virtual void PickUpEvent_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Equip();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Unequip();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
		void DrawSheath();
	void DrawSheath_Implementation();

	FTimerHandle DrawSheathTimer;

	void ResetAnimVariables();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
		void Attack();
	void Attack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
		void ChainAttack();
	void ChainAttack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
		void ResetAttack();
	void ResetAttack_Implementation();

	
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void StopMontage();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void LightAttackInput();
	virtual void LightAttackInput_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void HeavyAttackInput();
	virtual void HeavyAttackInput_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Base")
		virtual void UpdateHealthBar() { return; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
		void DealDamage(const float& Damage, const AActor* Sender, bool& Success);
	virtual void DealDamage_Implementation(const float& Damage, const AActor* Sender, bool& Success) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		FVector CurrentLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		FVector WeaponLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		FRotator WeaponRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		class ASkeletalMeshActor* Clone = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		class UParticleSystemComponent* ParticleSystem = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		class UMaterialParameterCollection* Collection = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
		UMaterialInterface* Material = nullptr;

	//Warp timeline variables
	class UTimelineComponent* WarpTimeline = nullptr;

	UTimelineComponent* BlueMeshLerp = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Warp")
		class UCurveFloat* fCharacterCurve = nullptr; 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Warp")
		UCurveFloat* fSwordCurve = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Warp")
		UCurveFloat* fFOVCurve = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Warp")
		UCurveFloat* fGlow = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Warp")
		TSubclassOf<UCameraShake> CameraShake;

	//Timeline functions
	UFUNCTION()
		void TimelineFloatReturn(float value);

	UFUNCTION()
		void OnTimelineFinished();

	UFUNCTION()
		void ReduceGlow(float value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Warp")
		void BeginWarp();
	virtual void BeginWarp_Implementation();

	UFUNCTION(Category = "Warp")
		void ContinueWarp();

	UFUNCTION(Category = "Warp")
		void DelayBoolSet();

	UFUNCTION(Category = "Warp")
		void FinishWarp();

	UFUNCTION(BlueprintCallable, Category = "Warp")
		ASkeletalMeshActor* CreateSkeletalMesh(float InPos);

	FTimerHandle WarpTimer;

	FTimerHandle WarpDelay;


	void WarpBoolSets(bool NewCanWarp = false, bool NewIsInWarp = false, bool NewCanMove = false);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UAIPerceptionStimuliSourceComponent* AIStimulusComponent = nullptr;
};

