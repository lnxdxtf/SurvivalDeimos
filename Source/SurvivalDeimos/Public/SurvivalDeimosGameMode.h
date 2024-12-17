// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SurvivalDeimosGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALDEIMOS_API ASurvivalDeimosGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerHud")
	TSubclassOf<class UUserWidget> PlayerHud;

	UPROPERTY()
	class UUserWidget* PlayerHudInstance;
};
