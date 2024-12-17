// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTPS.generated.h"

// Forward declare this class, so the header file knows it is valid
class UNiagaraSystem;

UCLASS()
class SURVIVALDEIMOS_API ACharacterTPS : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterTPS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// VFX ---------------------------------------------------------------
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	// UNiagaraSystem* TentacleVfx;

	// Character appearance ------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	class USkeletalMesh* DefaultSkeletalMesh;

	// Camera --------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	class UCameraComponent* CameraCharacterComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	class USpringArmComponent* CameraSpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float FZoomMin = 300.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float FZoomMax = 600.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float FZoomStep = 20.0f;
	void ZoomIn();
	void ZoomOut();

	// Movements -----------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category="Movement")
	float FSpeedWalk = 300.0f;
	UPROPERTY(VisibleAnywhere, Category="Movement")
	float FSpeedRun = 2.4 * FSpeedWalk;
	// Handle the player's movement (Get direction, check if the player is walking, set speed, etc)
	void CrouchToggle();
	void MoveY(float Value);
	void MoveX(float Value);
	void StopRun();
	void StartRun();

	// Stamina ------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FTotalStamina = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FCurrentStamina = FTotalStamina;
	// Stamina cost base for all actions (run, jump, etc) | WALK DOESN'T COST STAMINA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FGeneralStaminaCost = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FStaminaRunMultiplier = 10.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FStaminaJumpMultiplier = 15.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float FStaminaRegenMultiplier = 5.0f;
	FTimerHandle StaminaTimerHandle;

	// Check if the player has enough stamina to perform an action
	// Value: Stamina cost of the action
	bool HasStamina(float Value);
	// Check if the player has enough stamina to perform actions that consume stamina
	void HandleStaminaActions();

	// Weapon -------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<class AWeapon> TWeapon;
	void Fire();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bAttacking = false;
	FTimerHandle AttackTimerHandle;
	void StopAttacking();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bInCombat = false;
	FTimerHandle CombatTimerHandle;
	void ExitCombat();

public:
	class AWeapon* Weapon;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Health System -------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	class UHealthSystem* HealthSystem;
	UFUNCTION()
	void Down();
	UFUNCTION()
	void Dead();
};
