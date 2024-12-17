// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HealthSystem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALDEIMOS_API UHealthSystem : public UObject
{
	GENERATED_BODY()

public:
	UHealthSystem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	bool bIsDown = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	bool bIsImmortal = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UFUNCTION(BLueprintCallable, Category = "Health")
	void TakeDamage(float DamageValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealValue);

	// Time to revive In Seconds
	UPROPERTY(EditAnywhere, Category="Health")
	int TimeToRevive = 5;
	int TimeToReviveCounter = 0;
	FTimerHandle ReviveTimerHandle;
	UFUNCTION()
	void ReviveChecker();
	UFUNCTION()
	void Revive();
	UFUNCTION()
	void Dead();


	UPROPERTY()
	AActor* OwnerActor;
};
