// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthSystem.h"
#include "GameFramework/Character.h"
#include "BotCharacter.generated.h"

UCLASS()
class SURVIVALDEIMOS_API ABotCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABotCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class AWeapon> TWeapon;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class AWeapon* Weapon;

	// Health System -------------------------------------------------------
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
	UHealthSystem* HealthSystem;
	UFUNCTION()
	void Down();
	UFUNCTION()
	void Dead();
};
