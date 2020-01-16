// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "InventoryComponent.h"
#include "Quest.h"
#include "WeaponMaster.h"
#include "HealthStamina.h"


APlayerCharacter::APlayerCharacter()
{
	Cone = CreateDefaultSubobject<UStaticMeshComponent>(FName("Cone"));
	Cone->SetupAttachment(RootComponent);

	Cone->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginConeOverlap);
	Cone->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::EndConeOverlap);

	InteractionField = CreateDefaultSubobject<UCapsuleComponent>(FName("InteractionField"));
	InteractionField->SetupAttachment(RootComponent);

	InteractionField->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginInteractionFieldOverlap);
	InteractionField->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::EndInteractionFieldOverlap);
}


void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Custom input functionality
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::SprintEnd);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (wHealthStaminaBar)
	{
		HealthStaminaWidget = CreateWidget<UHealthStamina>(UGameplayStatics::GetPlayerController(GetWorld(), 0), wHealthStaminaBar, TEXT("HealthStaminaWidget"));
		if (HealthStaminaWidget)
		{
			HealthStaminaWidget->AddToViewport();
			UpdateHealthBar();
			UpdateStaminaBar();
		}
	}

}


bool APlayerCharacter::CheckStamina(int32 Threshold)
{
	return CurrentStamina > Threshold;
}

void APlayerCharacter::UpdateHealthBar()
{
	if (HealthStaminaWidget)
	{
		HealthStaminaWidget->UpdateHealth((float)CurrentHealth / (float)MaxHealth);
	}
}

void APlayerCharacter::UpdateStaminaBar()
{
	if (HealthStaminaWidget)
	{
		HealthStaminaWidget->UpdateStamina((float)CurrentStamina / (float)MaxStamina);
	}
}

void APlayerCharacter::SprintStart_Implementation()
{
	if (GetVelocity().SizeSquared() > 0.0f &&  CurrentStamina > 1 && !PlayingAnim)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetWorld()->GetTimerManager().SetTimer(SprintStaminaDecreaseHandle, this, &APlayerCharacter::SprintStaminaDecrease, .1f, true);
	}
}

void APlayerCharacter::SprintEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetWorld()->GetTimerManager().PauseTimer(SprintStaminaDecreaseHandle);
}

void APlayerCharacter::StartStopRegeneratingStamina_Implementation()
{
	GetWorld()->GetTimerManager().PauseTimer(RegenerateStaminaHandle);

	GetWorld()->GetTimerManager().SetTimer(RegenerateStaminaHandle, this, &APlayerCharacter::RegenerateStamina, .1f, true, 2.0f);
}



void APlayerCharacter::SprintStaminaDecrease()
{
	if (GetVelocity().SizeSquared() > 0.0f &&  CurrentStamina > 1 && !PlayingAnim)
	{
		CurrentStamina -= 2;
		UpdateStaminaBar();
		StartStopRegeneratingStamina();
	}
	else
	{
		GetWorld()->GetTimerManager().PauseTimer(SprintStaminaDecreaseHandle);
	}

}

void APlayerCharacter::RegenerateStamina()
{
	if (CurrentStamina >= MaxStamina)
	{
		GetWorld()->GetTimerManager().PauseTimer(RegenerateStaminaHandle);
		CurrentStamina = MaxStamina;
	}
	else
	{
		CurrentStamina += StaminaRegenerateValue;
		UpdateStaminaBar();
	}
}

void APlayerCharacter::StaminaDecrease(int32 DecreasedStamina)
{
	CurrentStamina -= DecreasedStamina;
	UpdateStaminaBar();
	StartStopRegeneratingStamina();
}

void APlayerCharacter::Interact_Implementation(const AActor * Interactor, bool & Success)
{
	if (InteractMessageUI && InteractMessageUI->IsValidLowLevel())
	{
		TArray<AActor*> Actors;
		InteractionField->GetOverlappingActors(Actors);

		for (AActor* Elem : Actors)
		{
			IInteracts* ImplementsInteract = Cast<IInteracts>(Elem);
			if (ImplementsInteract && Elem != this)
			{
				ImplementsInteract->Execute_Interact(Elem, this, Success);
				break;
			}
		}
	}
}

void APlayerCharacter::LockOnEvent_Implementation()
{
	if (TargetLocked)
	{
		TargetLocked = false;
		bool Success = false;
		NearestTarget->LockOn(Success);
		GetWorld()->GetTimerManager().PauseTimer(ToggleLockOnHandle);
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	else
	{
		FVector2D ViewportSize;
		APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		int32 x, y;
		PController->GetViewportSize(x, y);
		//GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		ViewportSize.X = x;
		ViewportSize.Y = y;
		ViewportSize *= .5f;
		ClosestTargetDistance = ViewportSize.Size();

		TArray<AActor*> Overlapping;
		TSubclassOf<AActor> MasterEnemy = AMasterEnemy::StaticClass();
		Cone->GetOverlappingActors(Overlapping, MasterEnemy);
		for (AActor* Actor : Overlapping)
		{
			AMasterEnemy* Enemy = Cast<AMasterEnemy>(Actor);
			if (Enemy && Enemy->CanBeTargeted && Enemy->WasRecentlyRendered())
			{
				EnemyElement = Enemy;
				if (Enemy->PlayerToEnemyDistance < ClosestTargetDistance)
				{
					LockOnDone();
				}
			}
		}
		SetLockOnToTarget();
	}
}

void APlayerCharacter::LockOnDone_Implementation()
{
	NearestTarget = EnemyElement;
	ClosestTargetDistance = NearestTarget->PlayerToEnemyDistance;
}

void APlayerCharacter::SetLockOnToTarget_Implementation()
{
	FVector2D ViewportSize;

	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	ViewportSize *= .5f;
	if (ClosestTargetDistance <= ViewportSize.Size() && NearestTarget && NearestTarget->IsValidLowLevel())
	{
		bool Success = false;
		NearestTarget->LockOn(Success);
		GetWorld()->GetTimerManager().SetTimer(ToggleLockOnHandle, this, &APlayerCharacter::ToggleLockOn, .01f, true);
		TargetLocked = true;
	}
}

void APlayerCharacter::ToggleLockOn()
{
	FRotator Rotation;
	AController* NewController;
	NewControlRotation(Rotation, NewController);

	NewController->SetControlRotation(Rotation);
	MovementLockOn();
}

void APlayerCharacter::NewControlRotation(FRotator & Rotator, AController *& NewController)
{
	NewController = GetController();

	FVector EnemyLocation = NearestTarget->GetActorLocation();
	EnemyLocation.Z -= 65;
	FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation);
	FRotator NewRotator = UKismetMathLibrary::RInterpTo(Rot, NewController->GetControlRotation(), GetWorld()->DeltaTimeSeconds, .0001f);

	Rotator = NewRotator;
	Rotator.Roll = NewController->GetControlRotation().Roll;
}

void APlayerCharacter::MovementLockOn()
{
	if (IsWeaponEquipped)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), NearestTarget->GetActorLocation());
		LookAt.Roll = 0.0f;
		LookAt.Pitch = 0.0f;

		FRotator Interp = UKismetMathLibrary::RInterpTo(GetController()->GetControlRotation(), LookAt, .01f, 1.0f);
		GetController()->SetControlRotation(Interp);
	}
}



void APlayerCharacter::BeginConeOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMasterEnemy* Enemy = Cast<AMasterEnemy>(OtherActor);
	if (Enemy && Enemy->IsValidLowLevel())
	{
		Enemy->CanBeTargeted = true;

		GetWorld()->GetTimerManager().SetTimer(Enemy->DistanceToPlayerHandle, Enemy, &AMasterEnemy::DistanceToPlayer, .01f, true);
	}
}

void APlayerCharacter::EndConeOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AMasterEnemy* Enemy = Cast<AMasterEnemy>(OtherActor);

	if (Enemy && Enemy->IsValidLowLevel())
	{
		GetWorld()->GetTimerManager().PauseTimer(Enemy->DistanceToPlayerHandle);
		Enemy->CanBeTargeted = false;
		if (TargetLocked && OtherActor == NearestTarget)
		{
			LockOnEvent();
		}
	}
}

void APlayerCharacter::BeginInteractionFieldOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AWeaponMaster* Weapon = Cast<AWeaponMaster>(OtherActor);
	if (!Weapon && OtherActor != this)
	{
		if (Cast<IInteracts>(OtherActor))
		{
			if (!InteractMessageUI || !InteractMessageUI->IsValidLowLevel())
			{
				UClass* Class = LoadObject<UClass>(this, *FString("WidgetBlueprint'/Game/QuestSystem/QuestUI/UI_InteractMessage.UI_InteractMessage_C'"));


				InteractMessageUI = NewObject<UWidgetComponent>(OtherActor);
				InteractMessageUI->SetWidgetClass(Class);
				//InteractMessageUI->SetDrawSize({ 500.0f, 500.0f });
				InteractMessageUI->SetWidgetSpace(EWidgetSpace::Screen);
				//InteractMessageUI->SetDrawAtDesiredSize(true);
				FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);

				InteractMessageUI->AttachToComponent(OtherComp, Rules);
				InteractMessageUI->RegisterComponent();

			}
		}
		else
		{
			TArray<AActor*> Actors;
			OtherActor->GetAttachedActors(Actors);
			for (AActor* Actor : Actors)
			{

			}
		}
	}
}

void APlayerCharacter::EndInteractionFieldOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (InteractMessageUI && InteractMessageUI->IsValidLowLevel() && OtherActor != this && OtherActor->GetComponentByClass(UWidgetComponent::StaticClass()))
	{
		InteractMessageUI->DestroyComponent();
		InteractMessageUI = nullptr;
	}
}

void APlayerCharacter::SwitchToLeft_Implementation()
{
	if (TargetLocked)
	{
		bool Success = false;
		NearestTarget->LockOn(Success);
		ClosestTargetRotation = 0.0f;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		ClosestTargetDistance = (ViewportSize * .5f).Size();

		TSubclassOf<AActor> EnemyClass = AMasterEnemy::StaticClass();
		TArray<AActor*> Enemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Enemies);

		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			AMasterEnemy* Enemy = Cast<AMasterEnemy>(Enemies[i]);
			if (Enemy != NearestTarget && Enemy->CanBeTargeted)
			{
				EnemyElement = Enemy;
				if (EnemyElement->PlayerToEnemyDistance < ClosestTargetDistance)
				{
					float ElementLookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyElement->GetActorLocation()).Yaw;
					float CurrentLookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NearestTarget->GetActorLocation()).Yaw;

					if (ElementLookAt < 0)
					{
						ElementLookAt += 360;
					}
					if (CurrentLookAt < 0)
					{
						CurrentLookAt += 360;
					}

					float LookDiff = ElementLookAt - CurrentLookAt;
					if (LookDiff < 0)
					{
						LookDiff += 360;
					}


					if (LookDiff >= 180.0f && LookDiff <= 360.0f && LookDiff > ClosestTargetRotation)
					{
						ClosestTargetRotation = LookDiff;
						NewTarget = EnemyElement;
						FoundNewTarget = true;
					}
				}
			}
		}

		if (FoundNewTarget)
		{
			SwitchDone();
			SetLockOnToTarget();
			FoundNewTarget = false;
		}
		else
		{
			bool Success;
			NearestTarget->LockOn(Success);
		}
	}
}

void APlayerCharacter::SwitchToRight_Implementation()
{
	if (TargetLocked)
	{
		bool Success = false;
		NearestTarget->LockOn(Success);
		ClosestTargetRotation = 180.0f;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		ClosestTargetDistance = (ViewportSize * .5f).Size();

		TSubclassOf<AActor> EnemyClass = AMasterEnemy::StaticClass();
		TArray<AActor*> Enemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Enemies);

		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			AMasterEnemy* Enemy = Cast<AMasterEnemy>(Enemies[i]);
			if (Enemy != NearestTarget && Enemy->CanBeTargeted)
			{
				EnemyElement = Enemy;
				if (EnemyElement->PlayerToEnemyDistance < ClosestTargetDistance)
				{
					float ElementLookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyElement->GetActorLocation()).Yaw;
					float CurrentLookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NearestTarget->GetActorLocation()).Yaw;

					if (ElementLookAt < 0)
					{
						ElementLookAt += 360;
					}
					if (CurrentLookAt < 0)
					{
						CurrentLookAt += 360;
					}

					float LookDiff = ElementLookAt - CurrentLookAt;
					if (LookDiff < 0)
					{
						LookDiff += 360;
					}


					if (LookDiff >= 0.0f && LookDiff < 180.0f && LookDiff < ClosestTargetRotation)
					{
						ClosestTargetRotation = LookDiff;
						NewTarget = EnemyElement;
						FoundNewTarget = true;
					}
				}
			}
		}

		if (FoundNewTarget)
		{
			SwitchDone();
			SetLockOnToTarget();
			FoundNewTarget = false;
		}
		else
		{
			bool Success;
			NearestTarget->LockOn(Success);
		}
	}
}

void APlayerCharacter::SwitchDone()
{
	NearestTarget = NewTarget;
	ClosestTargetDistance = NearestTarget->PlayerToEnemyDistance;
}

void APlayerCharacter::DealDamage_Implementation(const float & Damage, const AActor * Sender, bool & Success)
{
	Super::DealDamage_Implementation(Damage, Sender, Success);
}
