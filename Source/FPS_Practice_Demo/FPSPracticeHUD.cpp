// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPracticeHUD.h"
#include "FPS_Practice_DemoCharacter.h"
#include "FPS_Practice_DemoGameMode.h"
#include "FPS_Practice_DemoGameState.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"

void AFPSPracticeHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr;
	AFPS_Practice_DemoGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>() : nullptr;
	AFPS_Practice_DemoCharacter* PlayerCharacter = GetOwningPawn() ? Cast<AFPS_Practice_DemoCharacter>(GetOwningPawn()) : nullptr;

	UFont* ScoreFont = GEngine ? GEngine->GetSmallFont() : nullptr;
	UFont* CrosshairFont = GEngine ? GEngine->GetMediumFont() : nullptr;
	UFont* VictoryFont = GEngine ? GEngine->GetLargeFont() : nullptr;

	if (PracticeGameState || GameMode)
	{
		const int32 CurrentScore = PracticeGameState ? PracticeGameState->GetCurrentScore() : GameMode->GetCurrentScore();
		const int32 TargetScoreToWin = PracticeGameState ? PracticeGameState->GetTargetScoreToWin() : GameMode->GetTargetScoreToWin();
		const int32 HitTargetCount = PracticeGameState ? PracticeGameState->GetHitTargetCount() : GameMode->GetHitTargetCount();
		const int32 EnemyKillCount = PracticeGameState ? PracticeGameState->GetEnemyKillCount() : GameMode->GetEnemyKillCount();
		const bool bHasWonGame = PracticeGameState ? PracticeGameState->HasWonGame() : GameMode->HasWonGame();

		const FString ScoreText = FString::Printf(TEXT("Score: %d / %d"), CurrentScore, TargetScoreToWin);
		const FString HitText = FString::Printf(TEXT("Targets Hit: %d"), HitTargetCount);
		const FString KillText = FString::Printf(TEXT("Enemies Killed: %d"), EnemyKillCount);
		const FString PromptText = bHasWonGame ? TEXT("Victory! Press R to restart.") : TEXT("Shoot targets to score.");

		DrawText(ScoreText, FLinearColor::White, 40.0f, 40.0f, ScoreFont, 1.0f, false);
		DrawText(HitText, FLinearColor::White, 40.0f, 65.0f, ScoreFont, 1.0f, false);
		DrawText(KillText, FLinearColor::White, 40.0f, 90.0f, ScoreFont, 1.0f, false);
		DrawText(PromptText, FLinearColor::White, 40.0f, 115.0f, ScoreFont, 1.0f, false);
	}

	if (PlayerCharacter)
	{
		const FString HealthText = FString::Printf(TEXT("Health: %.0f / %.0f"), PlayerCharacter->GetCurrentHealth(), PlayerCharacter->GetMaxHealth());
		DrawText(HealthText, FLinearColor::White, 40.0f, 140.0f, ScoreFont, 1.0f, false);
	}

	const FString CrosshairText(TEXT("+"));
	float CrosshairWidth = 0.0f;
	float CrosshairHeight = 0.0f;
	GetTextSize(CrosshairText, CrosshairWidth, CrosshairHeight, CrosshairFont, 1.2f);

	DrawText(
		CrosshairText,
		FLinearColor::White,
		(Canvas->ClipX - CrosshairWidth) * 0.5f,
		(Canvas->ClipY - CrosshairHeight) * 0.5f,
		CrosshairFont,
		1.2f,
		false);

	const bool bShouldDrawVictory = PracticeGameState ? PracticeGameState->HasWonGame() : (GameMode && GameMode->HasWonGame());
	if (bShouldDrawVictory)
	{
		const FString VictoryText(TEXT("Victory!"));
		float VictoryWidth = 0.0f;
		float VictoryHeight = 0.0f;
		GetTextSize(VictoryText, VictoryWidth, VictoryHeight, VictoryFont, 1.0f);

		DrawText(
			VictoryText,
			FLinearColor::Yellow,
			(Canvas->ClipX - VictoryWidth) * 0.5f,
			(Canvas->ClipY - VictoryHeight) * 0.5f - 60.0f,
			VictoryFont,
			1.0f,
			false);
	}
}
