// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPracticePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "FPS_Practice_Demo.h"

AFPSPracticePlayerState::AFPSPracticePlayerState()
{
	bReplicates = true;
}

void AFPSPracticePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPracticePlayerState, PlayerScore);
	DOREPLIFETIME(AFPSPracticePlayerState, KillCount);
	DOREPLIFETIME(AFPSPracticePlayerState, DeathCount);
	DOREPLIFETIME(AFPSPracticePlayerState, HitTargetCount);
	DOREPLIFETIME(AFPSPracticePlayerState, EnemyKillCount);
}

void AFPSPracticePlayerState::AddPlayerScore(int32 ScoreAmount)
{
	PlayerScore = FMath::Max(0, PlayerScore + ScoreAmount);
}

void AFPSPracticePlayerState::AddKill()
{
	++KillCount;
}

void AFPSPracticePlayerState::AddDeath()
{
	++DeathCount;
}

void AFPSPracticePlayerState::AddTargetHit()
{
	++HitTargetCount;
}

void AFPSPracticePlayerState::AddEnemyKill()
{
	++EnemyKillCount;
}

void AFPSPracticePlayerState::OnRep_PlayerStats()
{
	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("PlayerState updated: %s Score=%d Kills=%d Deaths=%d TargetHits=%d EnemyKills=%d"),
		*GetPlayerName(),
		PlayerScore,
		KillCount,
		DeathCount,
		HitTargetCount,
		EnemyKillCount);
}
