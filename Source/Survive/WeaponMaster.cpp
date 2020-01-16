// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponMaster.h"
#include "SurviveCharacter.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "MasterEnemy.h"
#include "Damagable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Public/TimerManager.h"

// Sets default values
AWeaponMaster::AWeaponMaster() : EquipSocket("WEAPON_R"), HolsterSocket("GUNBACK_R")
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Weapon = CreateDefaultSubobject<UStaticMeshComponent>(FName("Weapon"));
	//Weapon->SetupAttachment(RootComponent);

	SK_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SK_Weapon"));
	SK_Weapon->SetupAttachment(RootComponent);
	SetRootComponent(SK_Weapon);
	

}

// Called when the game starts or when spawned
void AWeaponMaster::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CharacterRef)
	{
		CharacterRef = Cast<ASurviveCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Character ref: %s"), (CharacterRef ? *CharacterRef->GetName() : TEXT("NULL")));
	
}

// Called every frame
void AWeaponMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

void AWeaponMaster::Interact_Implementation(const AActor * Interactor, bool & Success)
{
}

void AWeaponMaster::LightAttack_Implementation()
{
	if (CharacterRef->ChainLightAttack + 1 >= LightAttacks.Num())
	{
		CharacterRef->LastAttack = true;
	}
	CharacterRef->PlayAnimMontage(LightAttacks[CharacterRef->ChainLightAttack]);

	ComboLightAttack(CharacterRef->ChainLightAttack);

	CharacterRef->ChainLightAttack += 1;
	CharacterRef->ChainLightAttack %= LightAttacks.Num();
}

void AWeaponMaster::HeavyAttack_Implementation()
{
	if (CharacterRef->ChainHeavyAttack + 1 >= HeavyAttacks.Num())
	{
		CharacterRef->LastAttack = true;
	}
	CharacterRef->PlayAnimMontage(HeavyAttacks[CharacterRef->ChainHeavyAttack]);

	ComboHeavyAttack(CharacterRef->ChainHeavyAttack);

	CharacterRef->ChainHeavyAttack += 1;
	CharacterRef->ChainHeavyAttack %= HeavyAttacks.Num();
}

void AWeaponMaster::ComboLightAttack_Implementation(int32 AttackCount)
{
}

void AWeaponMaster::ComboHeavyAttack_Implementation(int32 AttackCount)
{
}

void AWeaponMaster::HitEvent_Implementation(const int32 & HitDamage, const FHitResult & Hit)
{
	Damage = HitDamage;
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurviveCharacter::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		DamagedActor = Cast<ASurviveCharacter>(Enemy);
		if (Enemy->GetClass() == Hit.Actor->GetClass() && !(AlreadyDamagedEnemies.Contains(DamagedActor)))
		{
			AlreadyDamagedEnemies.Add(DamagedActor);

			if (CharacterRef->LightAttack)
			{
				if (Cast<IDamagable>(DamagedActor))
				{
					bool Success;
					IDamagable::Execute_DealDamage(DamagedActor, Damage, CharacterRef, Success);
				}
			}
			else if (CharacterRef->HeavyAttack)
			{
				if (Cast<IDamagable>(DamagedActor))
				{
					bool Success;
					IDamagable::Execute_DealDamage(DamagedActor, (Damage * 1.3f), CharacterRef, Success);
				}
			}

		}
	}
}



void AWeaponMaster::AttackTrace_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Trace C++"));
}

void AWeaponMaster::StartTraceTimer_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimer,this, &AWeaponMaster::AttackTrace, .01f, true);
}

void AWeaponMaster::StopTraceTimer_Implementation()
{
	GetWorld()->GetTimerManager().PauseTimer(TraceTimer);
	AlreadyDamagedEnemies.Empty();
}




