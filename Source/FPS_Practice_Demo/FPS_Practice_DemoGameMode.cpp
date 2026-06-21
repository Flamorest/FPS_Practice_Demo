// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoGameMode.h"
#include "FPSBasicEnemy.h"
#include "FPSPracticePlayerState.h"
#include "FPS_Practice_Demo.h"
#include "FPSPracticeHUD.h"
#include "FPS_Practice_DemoGameState.h"
#include "ShootingTarget.h"

AFPS_Practice_DemoGameMode::AFPS_Practice_DemoGameMode()
{
	HUDClass = AFPSPracticeHUD::StaticClass();
	GameStateClass = AFPS_Practice_DemoGameState::StaticClass();
	PlayerStateClass = AFPSPracticePlayerState::StaticClass();
}

void AFPS_Practice_DemoGameMode::InitGameState()
{
	Super::InitGameState();
	SyncGameState();
}

void AFPS_Practice_DemoGameMode::AddScoreForPlayer(AController* ScoringController, int32 ScoreAmount, AActor* ScoredTarget)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bHasWonGame)
	{
		return;
	}

	AFPSPracticePlayerState* ScoringPlayerState = GetPracticePlayerState(ScoringController);
	if (!ScoringPlayerState)
	{
		return;
	}

	if (ScoredTarget)
	{
		if (ScoredTarget->IsA<AShootingTarget>())
		{
			ScoringPlayerState->AddTargetHit();
		}
		else if (ScoredTarget->IsA<AFPSBasicEnemy>())
		{
			ScoringPlayerState->AddEnemyKill();
		}
	}

	const int32 ClampedScoreAmount = FMath::Clamp(ScoreAmount, 0, TargetScoreToWin - ScoringPlayerState->GetPlayerScore());
	ScoringPlayerState->AddPlayerScore(ClampedScoreAmount);

	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("Player Score Updated: Player=%s Score=%d / %d Target=%s TargetHits=%d EnemyKills=%d"),
		*ScoringPlayerState->GetPlayerName(),
		ScoringPlayerState->GetPlayerScore(),
		TargetScoreToWin,
		*GetNameSafe(ScoredTarget),
		ScoringPlayerState->GetHitTargetCount(),
		ScoringPlayerState->GetEnemyKillCount());

	CheckVictory(ScoringPlayerState);
	SyncGameState();
}

void AFPS_Practice_DemoGameMode::AddScoreForActor(AActor* ScoringActor, int32 ScoreAmount, AActor* ScoredTarget)
{
	if (!ScoringActor)
	{
		return;
	}

	AController* ScoringController = nullptr;
	if (const APawn* ScoringPawn = Cast<APawn>(ScoringActor))
	{
		ScoringController = ScoringPawn->GetController();
	}
	else
	{
		ScoringController = ScoringActor->GetInstigatorController();
	}

	AddScoreForPlayer(ScoringController, ScoreAmount, ScoredTarget);
}

void AFPS_Practice_DemoGameMode::AddPlayerKillScore(AController* KillerController, AController* VictimController)
{
	if (!HasAuthority() || bHasWonGame)
	{
		return;
	}

	if (!KillerController || !VictimController || KillerController == VictimController)
	{
		return;
	}

	AFPSPracticePlayerState* KillerPlayerState = GetPracticePlayerState(KillerController);
	AFPSPracticePlayerState* VictimPlayerState = GetPracticePlayerState(VictimController);
	if (!KillerPlayerState || !VictimPlayerState)
	{
		return;
	}

	KillerPlayerState->AddKill();
	VictimPlayerState->AddDeath();

	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("Player kill score added: Killer=%s Victim=%s Score=%d"),
		*KillerPlayerState->GetPlayerName(),
		*VictimPlayerState->GetPlayerName(),
		PlayerKillScore);

	AddScoreForPlayer(KillerController, PlayerKillScore, VictimController ? VictimController->GetPawn() : nullptr);
}

void AFPS_Practice_DemoGameMode::SyncGameState() const
{
	if (AFPS_Practice_DemoGameState* PracticeGameState = GetGameState<AFPS_Practice_DemoGameState>())
	{
		PracticeGameState->UpdateMatchState(TargetScoreToWin, bHasWonGame, WinningPlayerState);
	}
}

AFPSPracticePlayerState* AFPS_Practice_DemoGameMode::GetPracticePlayerState(AController* Controller) const
{
	return Controller ? Controller->GetPlayerState<AFPSPracticePlayerState>() : nullptr;
}

void AFPS_Practice_DemoGameMode::CheckVictory(AFPSPracticePlayerState* ScoringPlayerState)
{
	if (!ScoringPlayerState || bHasWonGame)
	{
		return;
	}

	if (ScoringPlayerState->GetPlayerScore() >= TargetScoreToWin)
	{
		bHasWonGame = true;
		WinningPlayerState = ScoringPlayerState;
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Victory"));
	}
}
