// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem.h"

#include "BotCharacter.h"
#include "CharacterTPS.h"

UHealthSystem::UHealthSystem()
{
}

void UHealthSystem::TakeDamage(float DamageValue)
{
	if (bIsImmortal)
	{
		return;
	}
	CurrentHealth <= 0 ? CurrentHealth = 0 : CurrentHealth -= DamageValue;
	bIsDown = CurrentHealth <= 0;
	if (bIsDown)
	{
		if (OwnerActor)
		{
			if (ACharacterTPS* CharacterTPS = Cast<ACharacterTPS>(OwnerActor))
			{
				CharacterTPS->Down();
			}
			else if (ABotCharacter* BotCharacter = Cast<ABotCharacter>(OwnerActor))
			{
				BotCharacter->Down();
			}

			OwnerActor->GetWorld()->GetTimerManager().SetTimer(ReviveTimerHandle, this
			                                                   , &UHealthSystem::ReviveChecker
			                                                   , 1, true);
		}
	}
}


void UHealthSystem::Heal(float HealValue)
{
	CurrentHealth += HealValue;
}

void UHealthSystem::ReviveChecker()
{
	if (TimeToReviveCounter >= TimeToRevive && bIsDown)
	{
		if (OwnerActor)
		{
			if (ACharacterTPS* CharacterTPS = Cast<ACharacterTPS>(OwnerActor))
			{
				CharacterTPS->Dead();
				CharacterTPS->GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
			}
			else if (ABotCharacter* BotCharacter = Cast<ABotCharacter>(OwnerActor))
			{
				BotCharacter->Dead();
				BotCharacter->GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
			}
		}
	}
	else
	{
		TimeToReviveCounter++;
	}
}

void UHealthSystem::Revive()
{
	if (bIsDown)
	{
		bIsDown = false;
		TimeToReviveCounter = 0;
		CurrentHealth = MaxHealth * 0.2f;
		OwnerActor->GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
	}
}

void UHealthSystem::Dead()
{
	if (bIsDown && OwnerActor)
	{
		OwnerActor->Destroy();
	}
}
