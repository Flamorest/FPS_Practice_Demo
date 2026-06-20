// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoGameMode.h"
#include "FPSBasicEnemy.h"
#include "FPS_Practice_Demo.h"
#include "FPSPracticeHUD.h"
#include "ShootingTarget.h"

AFPS_Practice_DemoGameMode::AFPS_Practice_DemoGameMode()
{
	HUDClass = AFPSPracticeHUD::StaticClass();
}

void AFPS_Practice_DemoGameMode::AddScore(int32 ScoreAmount, AActor* ScoredTarget)
{
	if (bHasWonGame)
	{
		return;
	}

	if (ScoredTarget)
	{
		if (ScoredTarget->IsA<AShootingTarget>())
		{
			++HitTargetCount;
		}
		else if (ScoredTarget->IsA<AFPSBasicEnemy>())
		{
			++EnemyKillCount;
		}
	}

	CurrentScore = FMath::Min(CurrentScore + ScoreAmount, TargetScoreToWin);

	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("Current Score: %d / %d (Target: %s, Hits: %d, Kills: %d)"),
		GetCurrentScore(),
		TargetScoreToWin,
		*GetNameSafe(ScoredTarget),
		HitTargetCount,
		EnemyKillCount);

	if (CurrentScore >= TargetScoreToWin)
	{
		bHasWonGame = true;
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Victory"));
	}
}
