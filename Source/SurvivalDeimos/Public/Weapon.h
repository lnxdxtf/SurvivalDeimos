// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// Forward declare this class, so the header file knows it is valid
class UNiagaraSystem;

UCLASS()
class SURVIVALDEIMOS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class USkeletalMeshComponent* WeaponSkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* MuzzleFlashVfx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* ImpactVfx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	class UParticleSystem* ImpactBloodVfx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	class UMaterialInterface* ImpactDecalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	class UMaterialInterface* ImpactDecalMaterialBlood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SFX")
	class USoundBase* FireSound;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Fire();
};
