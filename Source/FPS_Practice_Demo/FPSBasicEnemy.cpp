// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSBasicEnemy.h"
#include "FPS_Practice_Demo.h"
#include "FPS_Practice_DemoGameMode.h"

AFPSBasicEnemy::AFPSBasicEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

void AFPSBasicEnemy::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0.0f)
	{
		Die(DamageCauser);
	}
}

bool AFPSBasicEnemy::IsDead_Implementation() const
{
	return bIsDead;
}

void AFPSBasicEnemy::Die(AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetLifeSpan(2.0f);

	if (AFPS_Practice_DemoGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>())
	{
		GameMode->AddScore(ScoreValue, this);
	}

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy Dead: %s (Causer: %s)"), *GetNameSafe(this), *GetNameSafe(DamageCauser));
}
