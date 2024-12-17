// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BotCharacter_AIController.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALDEIMOS_API ABotCharacter_AIController : public AAIController
{
	GENERATED_BODY()

protected:
	ABotCharacter_AIController();

	virtual void OnPossess(APawn* InPawn) override;

	// Reponsible for sensing (seeing, hearing) other pawns
	UPROPERTY(EditDefaultsOnly)
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(EditDefaultsOnly)
	class UBlackboardComponent* BlackboardComponent;

	UFUNCTION()
	void OnPawnSeen(class APawn* SeenPawn);
};
