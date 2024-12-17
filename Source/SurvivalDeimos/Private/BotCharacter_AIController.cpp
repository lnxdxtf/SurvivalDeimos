// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter_AIController.h"

#include "BotCharacter.h"
#include "CharacterTPS.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Weapon.h"

ABotCharacter_AIController::ABotCharacter_AIController()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(
		TEXT("PawnSensingComponent"));
	PawnSensingComponent->SensingInterval = .25f; // 1/4 = 0.25 ...
	PawnSensingComponent->bOnlySensePlayers = true;
	PawnSensingComponent->SetPeripheralVisionAngle(180.f);
	PawnSensingComponent->SightRadius = 3000.f;

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(
		TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABotCharacter_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ABotCharacter_AIController::OnPawnSeen);

	if (BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
		BlackboardComponent->SetValueAsBool("Patrol", true);
	}
}

void ABotCharacter_AIController::OnPawnSeen(class APawn* SeenPawn)
{
	if (const ABotCharacter* BotCharacter = Cast<ABotCharacter>(GetPawn()))
	{
		ACharacterTPS* TargetCharacter = Cast<ACharacterTPS>(SeenPawn);
		if (BotCharacter->HealthSystem->bIsDown || TargetCharacter->HealthSystem->bIsDown)
		{
			BlackboardComponent->SetValueAsObject("Target", nullptr);
		}
		else
		{
			if (BlackboardComponent && TargetCharacter)
			{
				BlackboardComponent->SetValueAsBool("Patrol", false);
				BlackboardComponent->SetValueAsObject("Target", TargetCharacter);
				BotCharacter->Weapon->Fire();
			}
		}
	}
}
