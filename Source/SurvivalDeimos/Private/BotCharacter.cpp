// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter.h"

#include "AIController.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"

// Sets default values
ABotCharacter::ABotCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Skeletal Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultSkeletalMeshAsset(
		TEXT(
			"/Script/Engine.SkeletalMesh'/Game/ParagonMurdock/Characters/Heroes/Murdock/Skins/CS_Magma/Mesh/Murdock_Magma.Murdock_Magma'"));
	if (DefaultSkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultSkeletalMeshAsset.Object);
	}

	// Set position mesh component
	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

// Called when the game starts or when spawned
void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Health System
	HealthSystem = NewObject<UHealthSystem>();
	// Need this to set the owner of the HealthSystem, to handle the destroy of the actor
	HealthSystem->OwnerActor = this;

	FActorSpawnParameters SpawnWeaponParams;
	SpawnWeaponParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AWeapon>(TWeapon, FTransform(), SpawnWeaponParams);
	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale
		                          , FName("VB root_gunSocket"));
	}
}

void ABotCharacter::Down()
{
	// Disable Input
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		// Remove capsule collision
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AIController->StopMovement();
	}
}

void ABotCharacter::Dead()
{
	if (Weapon)
	{
		Weapon->Destroy();
	}
	Destroy();
}

// Called every frame
void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABotCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
