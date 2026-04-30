// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoGameMode.h"
#include "FPS_Practice_Demo.h"
#include "FPSPracticeHUD.h"

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

	CurrentScore = FMath::Min(CurrentScore + ScoreAmount, TargetScoreToWin);
	++HitTargetCount;

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Current Score: %d / %d (Target: %s, Hits: %d)"), GetCurrentScore(), TargetScoreToWin, *GetNameSafe(ScoredTarget), HitTargetCount);

	if (CurrentScore >= TargetScoreToWin)
	{
		bHasWonGame = true;
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Victory"));
	}
}
