// Fill out your copyright notice in the Description page of Project Settings.

#include "MasterEnemy.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealth.h"
#include "UObject/ConstructorHelpers.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Quest.h"
#include "QuestLog.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Public/TimerManager.h"
#include "EnemyBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"
#include "WeaponMaster.h"

// Sets default values
AMasterEnemy::AMasterEnemy()
{
	MaxHealth = 250;
	CurrentHealth = 200;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBarComponent->SetupAttachment(RootComponent);
	//HealthBarComponent->AddRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	HealthBarComponent->SetVisibility(true);
	
	WarpLocation = CreateDefaultSubobject<USceneComponent>("WarpLocation");
	WarpLocation->SetupAttachment(RootComponent);

	CrosshairWidgetA = CreateDefaultSubobject<UWidgetComponent>("CrosshairWidget");
	CrosshairWidgetA->SetupAttachment(WarpLocation);

	LockOnSphere = CreateDefaultSubobject<UStaticMeshComponent>("LockOnSphere");
	LockOnSphere->SetupAttachment(RootComponent);

	//Timeline creation
	CrosshairTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	

}

// Called when the game starts or when spawned
void AMasterEnemy::BeginPlay()
{
	Super::BeginPlay();

	
	
	EnemyHealthBar = Cast<UEnemyHealth>(HealthBarComponent->GetUserWidgetObject());// CreateWidget<UEnemyHealth>(GetGameInstance(), wHealthBar);
	CharacterRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (EnemyHealthBar->Health)
	{
		UpdateHealthBar();
		
		//EnemyHealthBar->AddToViewport();
		//EnemyHealthBar->AddToPlayerScreen();
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Health progress bar is nullptr"));
	}
	
	FOnTimelineFloat InterpFunction;
	FOnTimelineEvent TimelineFinished;

	InterpFunction.BindUFunction(this, TEXT("CrosshairTimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, TEXT("OnCrosshairTimelineFinished"));
	if (fCurve)
	{
		// Add the float curve to the timeline and connect it to the interp function's delegate
		CrosshairTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));
		// Add delegate for timeline finish
		CrosshairTimeline->SetTimelineFinishedFunc(TimelineFinished);
		
		CrosshairTimeline->SetLooping(false);
		CrosshairTimeline->SetIgnoreTimeDilation(false);
		CrosshairTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		CrosshairTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
	}

	PickUpEvent();

	EquippedWeapon->UpdateCharacterRef(this);
	/*if (EquippedWeapon)
	{
		DrawSheath();
	}*/
}

// Called every frame
void AMasterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CharacterRef)
	{
		FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(HealthBarComponent->GetComponentLocation(), CharacterRef->GetFollowCamera()->GetComponentLocation());
		HealthBarComponent->SetWorldRotation(LookAt);

	}
}

// Called to bind functionality to input
void AMasterEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMasterEnemy::UpdateHealthBar()
{
	if(!EnemyHealthBar)
	{
		return;
	}

	//Update AI Blackboard
	AEnemyBaseController* AIController = Cast<AEnemyBaseController>(GetController());

	if (AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsFloat("Health", CurrentHealth);
	}

	EnemyHealthBar->Update((float)CurrentHealth/(float)MaxHealth);
	if (CurrentHealth <= 0)
	{
		Death();
	}
}

void AMasterEnemy::DealDamage_Implementation(const float & Damage, const AActor * Sender, bool & Success)
{
	Super::DealDamage_Implementation(Damage, Sender, Success);
}

void AMasterEnemy::Death_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling c++ death!"));

	CanBeTargeted = false;
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	if (CharacterRef && CharacterRef->IsValidLowLevel())
	{
		GetMesh()->AddForceAtLocation(CharacterRef->GetActorForwardVector() * 500000.0f, GetActorLocation(), FName("head"));
		UQuestLog* Log = CharacterRef->GetQuestLog();
		for (int32 i = 0; i < Log->QuestsA.Num(); i++)
		{
			Log->QuestsA[i]->CheckKill(this);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AMasterEnemy::OnTimerEnd, 2.0f, false);
}

void AMasterEnemy::GetPlayer_Implementation()
{
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool Hit = ActorLineTraceSingle(OutHit,
						 GetActorLocation(),
						 ((CharacterRef && CharacterRef->IsValidLowLevel()) ? CharacterRef->GetActorLocation() : FVector(0.0f, 0.0f, 0.0f)),
						 ECollisionChannel::ECC_Visibility,
						 Params);
	
	if ((OutHit.TraceStart - OutHit.TraceEnd).Size() >= 300.0f)
	{
		WarpLocation->SetVisibility(true);
		CrosshairWidgetA->SetVisibility(true);
		FVector CrosshairDirection = (OutHit.TraceEnd - GetActorLocation()).GetSafeNormal();


		WarpLocation->SetWorldLocationAndRotation(FVector(GetActorLocation().X + CrosshairDirection.X * 100.0f,
														  GetActorLocation().Y + CrosshairDirection.Y * 100.0f,
														  WarpLocation->GetComponentLocation().Z),
												  UKismetMathLibrary::FindLookAtRotation(WarpLocation->GetComponentLocation(),
																						 OutHit.TraceEnd));
		FRotator CrosshairRotation = CrosshairWidgetA->GetComponentRotation();
		CrosshairRotation.Roll += .5f;
		CrosshairWidgetA->SetWorldLocationAndRotation(FVector(GetActorLocation().X + CrosshairDirection.X * 30.0f,
															 GetActorLocation().Y + CrosshairDirection.Y * 30.0f,
															 CrosshairWidgetA->GetComponentLocation().Z),
													 CrosshairRotation);
	}
	else
	{
		WarpLocation->SetVisibility(false);
		CrosshairWidgetA->SetVisibility(false);
	}
}

void AMasterEnemy::DistanceToPlayer_Implementation()
{
	int32 x, y;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->GetViewportSize(x, y);

	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);
	
	x *= .5f;
	y *= .5f;
	x -= ScreenLocation.X;
	y -= ScreenLocation.Y;

	FVector2D Location(x, y);
	PlayerToEnemyDistance = Location.Size();
}

void AMasterEnemy::Interact_Implementation(const AActor * Interactor, bool & Success)
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	AttachedActors.Remove(Player);

	for (AActor* Elem : AttachedActors)
	{
		AQuest* Quest = Cast<AQuest>(Elem);

		if (Quest)
		{
			UQuestLog* Log = Player->GetQuestLog();
			if (Log->QuestsA.Contains(Quest))
			{
				//Add quest UI to player
				Player->AddQuestUI(Quest);
			}
		}
		else
		{
			IInteracts* ImplementsInteract = Cast<IInteracts>(Elem);
			if (ImplementsInteract)
			{
				
				ImplementsInteract->Execute_Interact(Elem, Interactor, Success);
			}
		}
	}
	Success = true;
	return;
}



void AMasterEnemy::LockOn_Implementation(bool & Success)
{
	if (IsA)
	{
		ShowCross = true;
		LockOnSphere->SetVisibility(true);
		CrosshairWidgetA->SetVisibility(true);
		GetWorld()->GetTimerManager().SetTimer(GetPlayerHandle, this, &AMasterEnemy::GetPlayer, 0.01f, true);
		CrosshairTimeline->Play();
		
	}
	else
	{
		ShowCross = false;
		LockOnSphere->SetVisibility(false);
		//CrosshairWidgetA->SetVisibility(false);
		GetWorld()->GetTimerManager().PauseTimer(GetPlayerHandle);
		CrosshairTimeline->Reverse();

	}
	IsA = !IsA;
}

void AMasterEnemy::OnTimerEnd()
{
	Destroy();
}

void AMasterEnemy::CrosshairTimelineFloatReturn(float value)
{
	CrosshairWidgetA->SetWorldScale3D(FMath::InterpExpoInOut(CrosshairWidgetA->GetComponentScale(),
															(ShowCross ? MaxCrosshairScale : MinCrosshairScale),
															value));

}

void AMasterEnemy::OnCrosshairTimelineFinished()
{
	if (TimelineDirection == ETimelineDirection::Backward)
	{
		CrosshairWidgetA->SetVisibility(false);
	}
}

