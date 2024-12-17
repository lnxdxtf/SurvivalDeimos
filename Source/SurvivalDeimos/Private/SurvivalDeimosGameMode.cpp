// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalDeimosGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterTPS.h"
#include "Blueprint/UserWidget.h"


void ASurvivalDeimosGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACharacterTPS* Player = Cast<
		ACharacterTPS>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerHud)
	{
		PlayerHudInstance = CreateWidget<UUserWidget>(GetWorld(), PlayerHud);

		if (PlayerHudInstance)
		{
			PlayerHudInstance->AddToViewport();
		}
	}
}
