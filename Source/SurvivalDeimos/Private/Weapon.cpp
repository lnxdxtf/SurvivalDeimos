// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"
// Niagara
#include "BotCharacter.h"
#include "CharacterTPS.h"
#include "HealthSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Animation/SkeletalMeshActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("WeaponSkeletalMeshComponent"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshAsset(
		TEXT(
			"/Script/Engine.SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4_X.SK_AR4_X'"));

	if (WeaponMeshAsset.Succeeded())
	{
		WeaponSkeletalMeshComponent->SetSkeletalMesh(WeaponMeshAsset.Object);
	}

	// LOAD VFXs (Particles)
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MuzzleFlashVfxAsset(
		TEXT(
			"/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Projectiles/Hits/NS_FireArrow_Hit.NS_FireArrow_Hit'"));
	if (MuzzleFlashVfxAsset.Succeeded())
	{
		MuzzleFlashVfx = MuzzleFlashVfxAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ImpactVfxAsset(
		TEXT(
			"/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Projectiles/Hits/NS_CursedArrow_Hit.NS_CursedArrow_Hit'"));
	if (ImpactVfxAsset.Succeeded())
	{
		ImpactVfx = ImpactVfxAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactBloodVfxAsset(
		TEXT(
			"/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone'"));
	if (ImpactBloodVfxAsset.Succeeded())
	{
		ImpactBloodVfx = ImpactBloodVfxAsset.Object;
	}

	// LOAD VFXs (Materials Decals)
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ImpactDecalMaterialAsset(
		TEXT(
			"/Script/Engine.Material'/Game/Weapon_Effects/Materials/Weapon/M_Impact_Decal.M_Impact_Decal'"));
	if (ImpactDecalMaterialAsset.Succeeded())
	{
		ImpactDecalMaterial = ImpactDecalMaterialAsset.Object;
		ImpactDecalMaterialBlood = ImpactDecalMaterialAsset.Object;
	}
	// -----------------------------
	// Load SFX (Sound)
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/Weapon_Effects/Audio/weapon_shot.weapon_shot'"));
	if (FireSoundAsset.Succeeded())
	{
		FireSound = FireSoundAsset.Object;
	}
	// -----------------------------

	RootComponent = WeaponSkeletalMeshComponent;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(WeaponSkeletalMeshComponent, TEXT("Muzzle"));

	ArrowComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ArrowComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Fire()
{
	if (ArrowComponent == nullptr)
	{
		return;
	}

	if (MuzzleFlashVfx)
	{
		// How im using SpawnSystemAttached, location and direction are not used in this case
		// Use of UNiagaraFunctionLibrary::SpawnSystemAttached is to spawn a Niagara system attached to a component
		// on the case, the component is the WeaponSkeletalMeshComponent and the socket is "Muzzle"
		const FVector Location = FVector(0.0f);
		const FRotator Direction = FRotator(0.0f);
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashVfx, WeaponSkeletalMeshComponent, TEXT("Muzzle"), Location,
			Direction, EAttachLocation::SnapToTarget, true);
	}

	const FVector Start = ArrowComponent->GetComponentLocation();
	const FVector Direction = ArrowComponent->GetComponentRotation().Vector();
	const FVector End = Start + Direction * 10000.0f;

	// Need store hit result
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	// Ignore self actor (ArrowComponent)
	CollisionParams.AddIgnoredActor(this);
	// Ignore owner actor (MeshWeapon)
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.bTraceComplex = true;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility
	                                                       , CollisionParams);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(ArrowComponent, FireSound, Start);
	}

	if (bHit)
	{
		if (const AActor* HitActor = HitResult.GetActor(); HitActor->GetClass()->IsChildOf(
			ACharacter::StaticClass()))
		{
			if (ImpactBloodVfx)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactBloodVfx
				                                         , HitResult.Location);
			}

			if (const ACharacterTPS* PlayerCharacter = Cast<ACharacterTPS>(HitActor))
			{
				if (PlayerCharacter->HealthSystem)
				{
					PlayerCharacter->HealthSystem->TakeDamage(10);
				}
			}
			else if (const ABotCharacter* BotCharacter = Cast<ABotCharacter>(HitActor))
			{
				if (BotCharacter->HealthSystem)
				{
					BotCharacter->HealthSystem->TakeDamage(10);
				}
			}
		}
		else
		{
			if (ImpactVfx)
			{
				// How im using SpawnSystemAtLocation, location and direction are used in this case
				// Use of UNiagaraFunctionLibrary::SpawnSystemAtLocation is to spawn a Niagara system at a location
				// on the case, the location is the HitResult.Location and the direction is the normal of the HitResult
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(), ImpactVfx, HitResult.Location);
				if (ImpactDecalMaterial)
				{
					const FVector SizeDecal = FVector(FMath::RandRange(10.0f, 50.0f));
					UGameplayStatics::SpawnDecalAttached(ImpactDecalMaterial, SizeDecal,
					                                     HitResult.GetComponent(), NAME_None
					                                     , HitResult.Location,
					                                     HitResult.ImpactNormal.Rotation(),
					                                     EAttachLocation::KeepWorldPosition
					                                     , 10.0F);
				}
			}
		}
	}
	// Draw debug line
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
}
