// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SurviveCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
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
#include "Engine/Public/TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/SkeletalMeshActor.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
//////////////////////////////////////////////////////////////////////////
// ASurviveCharacter

ASurviveCharacter::ASurviveCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Commented out because will get BP version from character at runtime for now
	//InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleSystem"));
	ParticleSystem->SetupAttachment(GetMesh());

	WarpTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("WarpTimeline"));

	BlueMeshLerp = CreateDefaultSubobject<UTimelineComponent>(FName("BlueMeshLerp"));

	AIStimulusComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(FName("AIStimulusComponent"));

	AIStimulusComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

void ASurviveCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	FOnTimelineFloat InterpFunction;
	FOnTimelineEvent TimelineFinished;

	InterpFunction.BindUFunction(this, TEXT("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, TEXT("OnTimelineFinished"));
	if (fCharacterCurve && fSwordCurve && fFOVCurve)
	{
		// Add the float curve to the timeline and connect it to the interp function's delegate
		WarpTimeline->AddInterpFloat(fCharacterCurve, InterpFunction, FName("Alpha"));
		// Add delegate for timeline finish
		WarpTimeline->SetTimelineFinishedFunc(TimelineFinished);

		WarpTimeline->SetLooping(false);
		WarpTimeline->SetIgnoreTimeDilation(false);
		WarpTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		//WarpTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
	}

	FOnTimelineFloat AlphaFunc;

	AlphaFunc.BindUFunction(this, TEXT("ReduceGlow"));
	if (fGlow)
	{
		BlueMeshLerp->AddInterpFloat(fGlow, AlphaFunc);

		BlueMeshLerp->SetLooping(false);
		BlueMeshLerp->SetIgnoreTimeDilation(false);
		BlueMeshLerp->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void ASurviveCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurviveCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurviveCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASurviveCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASurviveCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASurviveCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASurviveCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASurviveCharacter::OnResetVR);


	//Custom input functionality
	PlayerInputComponent->BindAction("LAttack", IE_Pressed, this, &ASurviveCharacter::LightAttackInput);
	PlayerInputComponent->BindAction("HAttack", IE_Pressed, this, &ASurviveCharacter::HeavyAttackInput);
}

void ASurviveCharacter::Interact_Implementation(const AActor * Interactor, bool & Success)
{
	
	////Interact(Interactor, Success);
	//UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *(this->GetDebugName(this)));
	//TArray<AActor*> Temp;
	//GetAttachedActors(Temp);
	////GetAllChildActors(Temp, false);

	//Temp.Remove(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//for (int32 i = 0; i < Temp.Num(); i++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Attached actor %i: %s"), i, *(Temp[i]->GetDebugName(Temp[i])));
	//}
}

void ASurviveCharacter::PickUpEvent_Implementation()
{
	if (AttachedWeapon.GetDefaultObject())
	{
		if (EquippedWeapon && EquippedWeapon->IsValidLowLevel())
		{
			EquippedWeapon->Destroy();
		}

		FTransform SocketTransform = GetMesh()->GetSocketTransform(AttachedWeapon.GetDefaultObject()->EquipSocket);
		EquippedWeapon = Cast<AWeaponMaster>(GetWorld()->SpawnActor(AttachedWeapon.Get(), &SocketTransform));
		IsWeaponSet = true;
		IsWeaponEquipped = true;
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

		EquippedWeapon->AttachToComponent(GetMesh(), Rules, EquippedWeapon->EquipSocket);
	}
}

void ASurviveCharacter::Equip()
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);

	EquippedWeapon->AttachToComponent(GetMesh(), Rules, EquippedWeapon->EquipSocket);

	IsWeaponEquipped = true;
}

void ASurviveCharacter::Unequip()
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);

	EquippedWeapon->AttachToComponent(GetMesh(), Rules, EquippedWeapon->HolsterSocket);

	IsWeaponEquipped = false;
}

void ASurviveCharacter::DrawSheath_Implementation()
{
	if (!PlayingAnim && IsWeaponSet)
	{
		PlayingAnim = true;
		CannotAttack = true;
		GetCharacterMovement()->SetActive(false);
		if (IsWeaponEquipped)
		{
			float TimeLeft = PlayAnimMontage(EquippedWeapon->UnequipAnim);
			GetWorld()->GetTimerManager().SetTimer(DrawSheathTimer,
												   this,
												   &ASurviveCharacter::ResetAnimVariables,
												   1.0f,
												   false,
												   TimeLeft);
		}
		else
		{
			float TimeLeft = PlayAnimMontage(EquippedWeapon->EquipAnim);
			GetWorld()->GetTimerManager().SetTimer(DrawSheathTimer,
												   this,
												   &ASurviveCharacter::ResetAnimVariables,
												   1.0f,
												   false,
												   TimeLeft);
		}
	}
}

void ASurviveCharacter::ResetAnimVariables()
{
	PlayingAnim = false;
	CannotAttack = false;
	GetCharacterMovement()->SetActive(true);
}



void ASurviveCharacter::Attack_Implementation()
{
	PlayingAnim = true;
	if (!LastAttack)
	{
		if (LightAttack)
		{
			if (CheckStamina(5))
			{
				StaminaDecrease(5);
				EquippedWeapon->LightAttack();
			}
			else
			{
				ResetAttack();
			}
		}
		else if (HeavyAttack)
		{
			if (CheckStamina(10))
			{
				StaminaDecrease(10);
				EquippedWeapon->HeavyAttack();
			}
			else
			{
				ResetAttack();
			}
		}
	}
}

void ASurviveCharacter::ChainAttack_Implementation()
{
	if (IsChainAttack)
	{
		IsChainAttack = false;
		Attack();
	}
}

//Resets flags and counters associated with attacks and combos
void ASurviveCharacter::ResetAttack_Implementation()
{
	LightAttack = false;
	HeavyAttack = false;
	IsChainAttack = false;
	LastAttack = false;
	PlayingAnim = false;
	ChainLightAttack = 0;
	ChainHeavyAttack = 0;
}


void ASurviveCharacter::StopMontage()
{
	StopAnimMontage(GetCurrentMontage());
	GetCharacterMovement()->SetActive(true);
	
}

void ASurviveCharacter::LightAttackInput_Implementation()
{
	if (EquippedWeapon && !CannotAttack)
	{
		if (IsWeaponEquipped)
		{
			if (LastAttack || HeavyAttack)
			{
				IsChainAttack = true;
				LightAttack = true;
				HeavyAttack = false;
			}
			else
			{
				LightAttack = true;
				HeavyAttack = false;
				Attack();
			}
		}
		else
		{
			DrawSheath();
		}
	}
}

void ASurviveCharacter::HeavyAttackInput_Implementation()
{
	if (EquippedWeapon && !CannotAttack)
	{
		if (IsWeaponEquipped)
		{
			if (LastAttack || LightAttack)
			{
				IsChainAttack = true;
				LightAttack = false;
				HeavyAttack = true;
			}
			else
			{
				LightAttack = false;
				HeavyAttack = true;
				Attack();
			}
		}
		else
		{
			DrawSheath();
		}
	}
}

void ASurviveCharacter::DealDamage_Implementation(const float & Damage, const AActor * Sender, bool & Success)
{
	CurrentHealth -= Damage;
	//UE_LOG(LogTemp, Warning, TEXT("Dealing damage C++"));

	UpdateHealthBar();
	Success = true;
}

void ASurviveCharacter::TimelineFloatReturn(float value)
{
	UE_LOG(LogTemp, Warning, TEXT("Updating timeline"));

	FVector ActorLocation = UKismetMathLibrary::VEase(CurrentLocation,
							  NearestTarget->WarpLocation->GetComponentLocation(),
							  fCharacterCurve->GetFloatValue(WarpTimeline->GetPlaybackPosition()),
							  EEasingFunc::Type::ExpoIn);
	SetActorLocation(ActorLocation);

	FVector WLocation = UKismetMathLibrary::VEase(WeaponLocation,
													  NearestTarget->WarpLocation->GetComponentLocation(),
													  fSwordCurve->GetFloatValue(WarpTimeline->GetPlaybackPosition()),
													  EEasingFunc::Type::ExpoIn);
	EquippedWeapon->SetActorLocationAndRotation(WLocation, WeaponRotation);

	FollowCamera->SetFieldOfView(fFOVCurve->GetFloatValue(WarpTimeline->GetPlaybackPosition()));
}

void ASurviveCharacter::OnTimelineFinished()
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	EquippedWeapon->AttachToComponent(GetMesh(), Rules, EquippedWeapon->EquipSocket);

	GetMesh()->SetVisibility(true, false);
	GetMesh()->GlobalAnimRateScale = 1.0f;
	GetMesh()->SetPosition(1.28f, true); //Dependant on animation

	// = LoadObject<UCameraShake>(this, *FString("Blueprint'/Game/ThirdPersonCPP/Blueprints/WarpCameraShake.WarpCameraShake'"));
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayCameraShake(CameraShake, 4.0f);

	GetWorld()->GetTimerManager().SetTimer(WarpDelay, this, &ASurviveCharacter::DelayBoolSet, 1.0f, false, .84f); //Delay dependant on animation
}

void ASurviveCharacter::ReduceGlow(float value)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, "BlueOpacityEffect", value);
}

void ASurviveCharacter::BeginWarp_Implementation()
{
	if (CanWarp && NearestTarget->IsValidLowLevel())
	{
		WarpBoolSets(false, true, false);

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NearestTarget->GetActorLocation()));

		HeavyAttack = true;

		GetWorld()->GetTimerManager().SetTimer(WarpTimer, this, &ASurviveCharacter::ContinueWarp, 1.0f, false, .5f);
	}
}

void ASurviveCharacter::ContinueWarp()
{
	CurrentLocation = GetActorLocation();

	if (EquippedWeapon && EquippedWeapon->IsValidLowLevel())
	{
		WeaponLocation = EquippedWeapon->GetActorLocation();
		WeaponRotation = EquippedWeapon->GetWeaponMesh()->GetComponentRotation();

		
		//Collection = LoadObject<UMaterialParameterCollection>(this, *FString("MaterialParameterCollection'/Game/ThirdPersonCPP/Blueprints/MeshEffects.MeshEffects'"));
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("BlueOpacityEffect"), .5f);

		GetMesh()->GlobalAnimRateScale = .3f;
		GetMesh()->SetVisibility(false);
		
		Clone = CreateSkeletalMesh(.5f);

		FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);

		//EquippedWeapon->RootComponent->DetachFromComponent(Rules);
		EquippedWeapon->DetachRootComponentFromParent();
		
		ParticleSystem->Activate(true);
		WarpTimeline->PlayFromStart();

	}
}

void ASurviveCharacter::DelayBoolSet()
{
	WarpBoolSets(true, false, true);

	GetWorld()->GetTimerManager().SetTimer(WarpDelay, this, &ASurviveCharacter::FinishWarp, 1.0, false, .2f);
}

void ASurviveCharacter::FinishWarp()
{
	Clone->Destroy();
	ParticleSystem->Deactivate();
	//TODO: Timeline for blue mesh
	BlueMeshLerp->PlayFromStart();

	HeavyAttack = false;
}

ASkeletalMeshActor * ASurviveCharacter::CreateSkeletalMesh(float InPos)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASkeletalMeshActor* Skeleton = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), GetMesh()->GetComponentTransform(), Params);
	
	Skeleton->GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Skeleton->GetSkeletalMeshComponent()->SetSkeletalMesh(GetMesh()->SkeletalMesh, true);

	//Material = LoadObject<UMaterialInterface>(this, *FString("Material'/Game/ThirdPersonCPP/Blueprints/MeshFade.MeshFade'"));
	
	TArray<UMaterialInterface*> MeshMaterials = Skeleton->GetSkeletalMeshComponent()->GetMaterials();
	
	for (int32 i = 0; i < MeshMaterials.Num(); i++)
	{
		Skeleton->GetSkeletalMeshComponent()->SetMaterial(i, Material);
	}
	Skeleton->GetSkeletalMeshComponent()->PlayAnimation(LoadObject<UAnimationAsset>(this, *FString("AnimSequence'/Game/AssassinRetarget/Attack_Move_Achilles.Attack_Move_Achilles'")), false);
	Skeleton->GetSkeletalMeshComponent()->SetPosition(InPos, true);
	Skeleton->GetSkeletalMeshComponent()->GlobalAnimRateScale = 0.0f;

	return Skeleton;
}

void ASurviveCharacter::WarpBoolSets(bool NewCanWarp, bool NewIsInWarp, bool NewCanMove)
{
	CanWarp = NewCanWarp;
	IsInWarp = NewIsInWarp;
	CanMove = NewCanMove;
}



void ASurviveCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASurviveCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASurviveCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASurviveCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASurviveCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASurviveCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASurviveCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
