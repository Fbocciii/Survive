// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponMaster.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVE_API ASword : public AWeaponMaster
{
	GENERATED_BODY()


public:
	ASword();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collison")
	class UCapsuleComponent* MainCollision = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
	TArray<class USphereComponent*> Spheres;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere1 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere2 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere3 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere4 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere5 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere6 = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Collison")
		USphereComponent* Sphere7 = nullptr;

	
		void ComboLightAttack(int32 AttackCount);
	virtual void ComboLightAttack_Implementation(int32 AttackCount) override;

		void ComboHeavyAttack(int32 AttackCount);
	virtual void ComboHeavyAttack_Implementation(int32 AttackCount) override;

		void AttackTrace();
	virtual void AttackTrace_Implementation() override;

	UFUNCTION()
		void EnemyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
