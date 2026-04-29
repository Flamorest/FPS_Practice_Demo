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
	CurrentScore += ScoreAmount;
	++HitTargetCount;

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Current Score: %d / %d (Target: %s, Hits: %d)"), CurrentScore, TargetScoreToWin, *GetNameSafe(ScoredTarget), HitTargetCount);

	if (HasWonGame())
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Victory"));
	}
}
