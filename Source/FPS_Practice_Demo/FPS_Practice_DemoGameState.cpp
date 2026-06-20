// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoGameState.h"
#include "Net/UnrealNetwork.h"
#include "FPS_Practice_Demo.h"

AFPS_Practice_DemoGameState::AFPS_Practice_DemoGameState()
{
	bReplicates = true;
}

void AFPS_Practice_DemoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_Practice_DemoGameState, CurrentScore);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, TargetScoreToWin);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, HitTargetCount);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, EnemyKillCount);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, bHasWonGame);
}

void AFPS_Practice_DemoGameState::UpdateScoreState(int32 NewCurrentScore, int32 NewTargetScoreToWin, int32 NewHitTargetCount, int32 NewEnemyKillCount, bool bNewHasWonGame)
{
	CurrentScore = NewCurrentScore;
	TargetScoreToWin = NewTargetScoreToWin;
	HitTargetCount = NewHitTargetCount;
	EnemyKillCount = NewEnemyKillCount;
	bHasWonGame = bNewHasWonGame;

	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("GameState score updated: Score %d / %d, Targets Hit %d, Enemies Killed %d, Victory %s"),
		CurrentScore,
		TargetScoreToWin,
		HitTargetCount,
		EnemyKillCount,
		bHasWonGame ? TEXT("true") : TEXT("false"));
}

void AFPS_Practice_DemoGameState::OnRep_ScoreState()
{
	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("GameState score updated: Score %d / %d, Targets Hit %d, Enemies Killed %d, Victory %s"),
		CurrentScore,
		TargetScoreToWin,
		HitTargetCount,
		EnemyKillCount,
		bHasWonGame ? TEXT("true") : TEXT("false"));
}
