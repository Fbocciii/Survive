// Fill out your copyright notice in the Description page of Project Settings.

#include "Sword.h"
#include "SurviveCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"

ASword::ASword() : AWeaponMaster()
{
	MainCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("MainCollision"));
	MainCollision->SetupAttachment(RootComponent);

	Sphere1 = CreateDefaultSubobject<USphereComponent>(FName("Sphere1"));
	Sphere1->SetupAttachment(RootComponent);
	Spheres.Add(Sphere1);
	Sphere1->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere2 = CreateDefaultSubobject<USphereComponent>(FName("Sphere2"));
	Sphere2->SetupAttachment(RootComponent);
	Spheres.Add(Sphere2);
	Sphere2->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere3 = CreateDefaultSubobject<USphereComponent>(FName("Sphere3"));
	Sphere3->SetupAttachment(RootComponent);
	Spheres.Add(Sphere3);
	Sphere3->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere4 = CreateDefaultSubobject<USphereComponent>(FName("Sphere4"));
	Sphere4->SetupAttachment(RootComponent);
	Spheres.Add(Sphere4);
	Sphere4->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere5 = CreateDefaultSubobject<USphereComponent>(FName("Sphere5"));
	Sphere5->SetupAttachment(RootComponent);
	Spheres.Add(Sphere5);
	Sphere5->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere6 = CreateDefaultSubobject<USphereComponent>(FName("Sphere6"));
	Sphere6->SetupAttachment(RootComponent);
	Spheres.Add(Sphere6);
	Sphere6->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

	Sphere7 = CreateDefaultSubobject<USphereComponent>(FName("Sphere7"));
	Sphere7->SetupAttachment(RootComponent);
	Spheres.Add(Sphere7);
	Sphere7->OnComponentBeginOverlap.AddDynamic(this, &ASword::EnemyOverlap);
	

}



void ASword::ComboLightAttack_Implementation(int32 AttackCount)
{
	if (AttackCount >= HeavyAttacks.Num())
	{
		CharacterRef->ChainHeavyAttack = AttackCount - HeavyAttacks.Num();
	}
	else
	{
		CharacterRef->ChainHeavyAttack = AttackCount;
	}
}

void ASword::ComboHeavyAttack_Implementation(int32 AttackCount)
{
	if (AttackCount >= LightAttacks.Num())
	{
		CharacterRef->ChainLightAttack = AttackCount - LightAttacks.Num();
	}
	else
	{
		CharacterRef->ChainLightAttack = AttackCount;
	}
}

void ASword::AttackTrace_Implementation()
{
	for (USphereComponent* Sphere : Spheres)
	{
		FHitResult Hit;
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(CharacterRef);
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterRef);
		Params.bTraceComplex = true;
		

		TArray<TEnumAsByte<EObjectTypeQuery>> Query;
		Query.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		bool WasHit = GetWorld()->LineTraceSingleByObjectType(Hit,
															  Sphere->GetComponentLocation(),
															  Sphere->GetComponentLocation() - (Sphere->GetComponentRotation().Vector().ForwardVector * Sphere->GetScaledSphereRadius()),
															  ObjectParams,
															  Params);

		/*bool WasHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
																	  Sphere->GetComponentLocation(),
																	  Sphere->GetComponentLocation() - (Sphere->GetComponentRotation().Vector().ForwardVector * Sphere->GetScaledSphereRadius()),
																	  Query,
																	  true,
																	  ToIgnore,
																	  EDrawDebugTrace::ForDuration,
																	  Hit,
																	  true,
																	  FLinearColor::Red,
																	  FLinearColor::Green,
																	  0.2f);*/
		if (WasHit)
		{
			//DrawDebugSphere(GetWorld(), Sphere->GetComponentLocation(), Sphere->GetScaledSphereRadius(), 32, FColor::Green, true, 1.0f);
			HitEvent(Damage, Hit);
		}
		else
		{
			//DrawDebugSphere(GetWorld(), Sphere->GetComponentLocation(), Sphere->GetScaledSphereRadius(), 32, FColor::Red, true, 1.0f);
		}
	}
}

void ASword::EnemyOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//DrawDebugSphere(GetWorld(), OverlappedComponent->GetComponentLocation(), 5.0f, 32, FColor::Red, true, .5f);
	//UE_LOG(LogTemp, Warning, TEXT("Overlapping weapon"));
}
