// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interacts.h"
#include "WeaponMaster.generated.h"

UCLASS()
class SURVIVE_API AWeaponMaster : public AActor, public IInteracts
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		TArray<UAnimMontage*> LightAttacks;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		TArray<UAnimMontage*> HeavyAttacks;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		class ASurviveCharacter* CharacterRef = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		int32 Damage = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		class ACharacter* DamagedActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		TArray<ACharacter*> AlreadyDamagedEnemies;
	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		class UStaticMeshComponent* Weapon = nullptr;*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		class USkeletalMeshComponent* SK_Weapon = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		FName EquipSocket;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		FName HolsterSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		class UAnimMontage* EquipAnim = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
		UAnimMontage* UnequipAnim = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor * Interactor, bool & Success);
	virtual void Interact_Implementation(const AActor * Interactor, bool & Success) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void LightAttack();
	virtual void LightAttack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void HeavyAttack();
	virtual void HeavyAttack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void ComboLightAttack(int32 AttackCount);
	virtual void ComboLightAttack_Implementation(int32 AttackCount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void ComboHeavyAttack(int32 AttackCount);
	virtual void ComboHeavyAttack_Implementation(int32 AttackCount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void HitEvent(const int32& HitDamage, const FHitResult& Hit);
	virtual void HitEvent_Implementation(const int32& HitDamage, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void AttackTrace();
	virtual void AttackTrace_Implementation();

	FTimerHandle TraceTimer;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void StartTraceTimer();
	virtual void StartTraceTimer_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attack")
		void StopTraceTimer();
	virtual void StopTraceTimer_Implementation();

	USkeletalMeshComponent* GetWeaponMesh() { return SK_Weapon; }

public:
	virtual void UpdateCharacterRef() { return; }  //Change variable to establish correct owning actor
	virtual void UpdateCharacterRef(ASurviveCharacter* NewRef) { CharacterRef = NewRef; }
};
