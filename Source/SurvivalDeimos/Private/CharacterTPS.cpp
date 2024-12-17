// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTPS.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Weapon.h"
#include "WorldCollision.h"
#include "UObject/ConstructorHelpers.h"
#include "HealthSystem.h"
// #include "NiagaraFunctionLibrary.h"
// #include "NiagaraComponent.h"

// Sets default values
ACharacterTPS::ACharacterTPS()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera
	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("CameraSpringArmCharacter"));
	CameraSpringArmComponent->TargetArmLength = FZoomMin * 1.2;
	CameraSpringArmComponent->bUsePawnControlRotation = true;
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 40.0f;
	CameraSpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	CameraSpringArmComponent->SetupAttachment(RootComponent);

	// Camera
	CameraCharacterComponent = CreateDefaultSubobject<
		UCameraComponent>(TEXT("CameraMainCharacter"));
	CameraCharacterComponent->SetupAttachment(CameraSpringArmComponent);

	// Possess Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Skeletal Mesh 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultSkeletalMeshAsset(
		TEXT(
			"/Script/Engine.SkeletalMesh'/Game/ParagonMurdock/Characters/Heroes/Murdock/Meshes/Murdock.Murdock'"));
	if (DefaultSkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultSkeletalMeshAsset.Object);
	}
	// VFX
	// static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TentacleVfxAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/NiagaraTentacles/Effects/NS_TentaclesUp3.NS_TentaclesUp3'"));
	// if (TentacleVfxAsset.Succeeded())
	// {
	// 	TentacleVfx = TentacleVfxAsset.Object;
	// }


	// Enable Crouch
	GetCharacterMovement()->CrouchedHalfHeight = 70.0f;
	ACharacter::GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Set position mesh component
	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// Set Base Stamina
	FCurrentStamina = FTotalStamina;

	// Set jump
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.05f;
}

// Called when the game starts or when spawned
void ACharacterTPS::BeginPlay()
{
	Super::BeginPlay();

	// Health System
	HealthSystem = NewObject<UHealthSystem>();
	HealthSystem->OwnerActor = this;

	FCurrentStamina = FTotalStamina;

	FActorSpawnParameters SpawnWeaponParams;
	SpawnWeaponParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AWeapon>(TWeapon, FTransform(), SpawnWeaponParams);
	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                          FName("VB root_gunSocket"));
	}


	// if (TentacleVfx)
	// {
	// 	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TentacleVfx, GetMesh(), TEXT("pelvis"), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), EAttachLocation::SnapToTarget, true);
	// }

	// Set Speed Default
	GetCharacterMovement()->MaxWalkSpeed = FSpeedWalk;
	GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this
	                                       , &ACharacterTPS::HandleStaminaActions, 1.0f, true);
}

void ACharacterTPS::MoveY(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ACharacterTPS::MoveX(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}


void ACharacterTPS::ZoomIn()
{
	CameraSpringArmComponent->TargetArmLength = FMath::Clamp(
		CameraSpringArmComponent->TargetArmLength - FZoomStep, FZoomMin, FZoomMax);
}

void ACharacterTPS::ZoomOut()
{
	CameraSpringArmComponent->TargetArmLength = FMath::Clamp(
		CameraSpringArmComponent->TargetArmLength + FZoomStep, FZoomMin, FZoomMax);
}


void ACharacterTPS::CrouchToggle()
{
	GetCharacterMovement()->IsCrouching()
		? UnCrouch()
		: Crouch();
}

void ACharacterTPS::StopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = FSpeedWalk;
}

void ACharacterTPS::StartRun()
{
	GetCharacterMovement()->MaxWalkSpeed = FSpeedRun;
}

bool ACharacterTPS::HasStamina(float Value)
{
	return FCurrentStamina >= Value;
}

void ACharacterTPS::HandleStaminaActions()
{
	if (GetCharacterMovement()->MaxWalkSpeed == FSpeedRun)
	{
		if (HasStamina(FStaminaRunMultiplier * FGeneralStaminaCost))
		{
			FCurrentStamina -= FStaminaRunMultiplier * FGeneralStaminaCost;
		}
		else
		{
			StopRun();
		}
	}

	if (IsJumpProvidingForce())
	{
		if (HasStamina(FStaminaJumpMultiplier * FGeneralStaminaCost))
		{
			FCurrentStamina -= FStaminaJumpMultiplier * FGeneralStaminaCost;
		}
		else
		{
			StopJumping();
		}
	}

	// Regen Stamina
	if (!IsJumpProvidingForce() && GetCharacterMovement()->MaxWalkSpeed == FSpeedWalk &&
		FCurrentStamina < FTotalStamina)
	{
		FCurrentStamina += FStaminaRegenMultiplier;
	}
}

void ACharacterTPS::Fire()
{
	if (Weapon)
	{
		Weapon->Fire();
		bInCombat = true;
		bAttacking = true;

		// Timer to exit combat
		GetWorld()->GetTimerManager().ClearTimer(CombatTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(CombatTimerHandle, this, &ACharacterTPS::ExitCombat
		                                       , 5.0f, false);

		// Timer to stop attacking
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this
		                                       , &ACharacterTPS::StopAttacking, 0.5f, false);
	}
}

void ACharacterTPS::StopAttacking()
{
	bAttacking = false;
}


void ACharacterTPS::ExitCombat()
{
	bInCombat = false;
	bAttacking = false;
}

void ACharacterTPS::Down()
{
	// Disable Input
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController);
	PlayerController->GetPawn()->DisableInput(PlayerController);
}

void ACharacterTPS::Dead()
{
	if (Weapon)
	{
		Weapon->Destroy();
	}
	Destroy();
}

// Called every frame
void ACharacterTPS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterTPS::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Move Player (X, Y)
	PlayerInputComponent->BindAxis("MoveY", this, &ACharacterTPS::MoveY);
	PlayerInputComponent->BindAxis("MoveX", this, &ACharacterTPS::MoveX);

	// Camera Move (X, Y)
	PlayerInputComponent->BindAxis("CameraMoveX", this, &ACharacterTPS::AddControllerYawInput);
	PlayerInputComponent->BindAxis("CameraMoveY", this
	                               , &ACharacterTPS::AddControllerPitchInput);
	// Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACharacterTPS::CrouchToggle);

	// Run
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ACharacterTPS::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ACharacterTPS::StopRun);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Fire Weapon
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterTPS::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Repeat, this, &ACharacterTPS::Fire);

	// Camera Zoom
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACharacterTPS::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACharacterTPS::ZoomOut);
}
