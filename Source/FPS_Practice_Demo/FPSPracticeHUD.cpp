// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPracticeHUD.h"
#include "FPSPracticePlayerState.h"
#include "FPS_Practice_DemoCharacter.h"
#include "FPS_Practice_DemoGameState.h"
#include "GameFramework/PlayerState.h"
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
	AFPS_Practice_DemoCharacter* PlayerCharacter = GetOwningPawn() ? Cast<AFPS_Practice_DemoCharacter>(GetOwningPawn()) : nullptr;
	AFPSPracticePlayerState* PracticePlayerState = GetOwningPlayerController() ? GetOwningPlayerController()->GetPlayerState<AFPSPracticePlayerState>() : nullptr;

	UFont* ScoreFont = GEngine ? GEngine->GetSmallFont() : nullptr;
	UFont* CrosshairFont = GEngine ? GEngine->GetMediumFont() : nullptr;
	UFont* VictoryFont = GEngine ? GEngine->GetLargeFont() : nullptr;

	if (PracticeGameState && PracticePlayerState)
	{
		const int32 CurrentScore = FMath::Min(PracticePlayerState->GetPlayerScore(), PracticeGameState->GetTargetScoreToWin());
		const int32 TargetScoreToWin = PracticeGameState->GetTargetScoreToWin();
		const int32 HitTargetCount = PracticePlayerState->GetHitTargetCount();
		const int32 EnemyKillCount = PracticePlayerState->GetEnemyKillCount();
		const bool bHasWonGame = PracticeGameState->HasWonGame();
		const bool bIsWinningPlayer = !bHasWonGame || (PracticeGameState->GetWinningPlayerState() == PracticePlayerState);

		const FString ScoreText = FString::Printf(TEXT("Score: %d / %d"), CurrentScore, TargetScoreToWin);
		const FString HitText = FString::Printf(TEXT("Targets Hit: %d"), HitTargetCount);
		const FString KillText = FString::Printf(TEXT("Enemies Killed: %d"), EnemyKillCount);
		const FString PromptText = !bHasWonGame
			? TEXT("Shoot targets to score.")
			: (bIsWinningPlayer ? TEXT("Victory! Press R to restart.") : TEXT("Defeat! Press R to restart."));

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

	if (PracticeGameState && PlayerCharacter && PlayerCharacter->ShouldDrawScoreboard())
	{
		const float PanelX = 40.0f;
		const float PanelY = 180.0f;
		const float PanelWidth = 420.0f;
		const float HeaderHeight = 28.0f;
		const float RowHeight = 22.0f;
		const int32 PlayerCount = PracticeGameState->PlayerArray.Num();
		const float PanelHeight = 16.0f + HeaderHeight + (FMath::Max(PlayerCount, 1) * RowHeight) + 16.0f;

		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), PanelX, PanelY, PanelWidth, PanelHeight);
		DrawText(TEXT("Scoreboard"), FLinearColor::White, PanelX + 16.0f, PanelY + 10.0f, ScoreFont, 1.0f, false);
		DrawText(TEXT("Player"), FLinearColor::Gray, PanelX + 16.0f, PanelY + 38.0f, ScoreFont, 1.0f, false);
		DrawText(TEXT("Score"), FLinearColor::Gray, PanelX + 190.0f, PanelY + 38.0f, ScoreFont, 1.0f, false);
		DrawText(TEXT("K"), FLinearColor::Gray, PanelX + 285.0f, PanelY + 38.0f, ScoreFont, 1.0f, false);
		DrawText(TEXT("D"), FLinearColor::Gray, PanelX + 335.0f, PanelY + 38.0f, ScoreFont, 1.0f, false);

		float RowY = PanelY + 62.0f;
		int32 DisplayIndex = 1;
		for (APlayerState* BasePlayerState : PracticeGameState->PlayerArray)
		{
			AFPSPracticePlayerState* ScorePlayerState = Cast<AFPSPracticePlayerState>(BasePlayerState);
			if (!ScorePlayerState)
			{
				continue;
			}

			FString PlayerLabel = ScorePlayerState->GetPlayerDisplayName();
			if (PlayerLabel.IsEmpty())
			{
				PlayerLabel = FString::Printf(TEXT("Player %d"), DisplayIndex);
			}

			DrawText(PlayerLabel, ScorePlayerState->GetPlayerColor(), PanelX + 16.0f, RowY, ScoreFont, 1.0f, false);
			DrawText(FString::Printf(TEXT("%d"), ScorePlayerState->GetPlayerScore()), FLinearColor::White, PanelX + 190.0f, RowY, ScoreFont, 1.0f, false);
			DrawText(FString::Printf(TEXT("%d"), ScorePlayerState->GetKillCount()), FLinearColor::White, PanelX + 285.0f, RowY, ScoreFont, 1.0f, false);
			DrawText(FString::Printf(TEXT("%d"), ScorePlayerState->GetDeathCount()), FLinearColor::White, PanelX + 335.0f, RowY, ScoreFont, 1.0f, false);

			RowY += RowHeight;
			++DisplayIndex;
		}
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

	if (PlayerCharacter && PlayerCharacter->ShouldDrawHitMarker())
	{
		const FVector2D ScreenCenter(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
		const float MarkerOffset = 12.0f;
		const float MarkerSize = 8.0f;
		const FLinearColor MarkerColor = FLinearColor::White;

		DrawLine(ScreenCenter.X - MarkerOffset, ScreenCenter.Y - MarkerOffset, ScreenCenter.X - MarkerOffset - MarkerSize, ScreenCenter.Y - MarkerOffset - MarkerSize, MarkerColor, 2.0f);
		DrawLine(ScreenCenter.X + MarkerOffset, ScreenCenter.Y - MarkerOffset, ScreenCenter.X + MarkerOffset + MarkerSize, ScreenCenter.Y - MarkerOffset - MarkerSize, MarkerColor, 2.0f);
		DrawLine(ScreenCenter.X - MarkerOffset, ScreenCenter.Y + MarkerOffset, ScreenCenter.X - MarkerOffset - MarkerSize, ScreenCenter.Y + MarkerOffset + MarkerSize, MarkerColor, 2.0f);
		DrawLine(ScreenCenter.X + MarkerOffset, ScreenCenter.Y + MarkerOffset, ScreenCenter.X + MarkerOffset + MarkerSize, ScreenCenter.Y + MarkerOffset + MarkerSize, MarkerColor, 2.0f);
	}

	if (PlayerCharacter && PlayerCharacter->ShouldDrawDamageFeedback())
	{
		const FString DamageText(TEXT("HIT!"));
		float DamageTextWidth = 0.0f;
		float DamageTextHeight = 0.0f;
		GetTextSize(DamageText, DamageTextWidth, DamageTextHeight, ScoreFont, 1.2f);

		DrawText(
			DamageText,
			FLinearColor(1.0f, 0.2f, 0.2f, 1.0f),
			(Canvas->ClipX - DamageTextWidth) * 0.5f,
			(Canvas->ClipY * 0.5f) + 48.0f,
			ScoreFont,
			1.2f,
			false);
	}

	const bool bShouldDrawMatchResult = PracticeGameState && PracticePlayerState && PracticeGameState->HasWonGame();
	if (bShouldDrawMatchResult)
	{
		const bool bIsWinningPlayer = PracticeGameState->GetWinningPlayerState() == PracticePlayerState;
		const FString ResultText = bIsWinningPlayer ? TEXT("Victory!") : TEXT("Defeat!");
		const FLinearColor ResultColor = bIsWinningPlayer ? FLinearColor::Yellow : FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);
		float ResultWidth = 0.0f;
		float ResultHeight = 0.0f;
		GetTextSize(ResultText, ResultWidth, ResultHeight, VictoryFont, 1.0f);

		DrawText(
			ResultText,
			ResultColor,
			(Canvas->ClipX - ResultWidth) * 0.5f,
			(Canvas->ClipY - ResultHeight) * 0.5f - 60.0f,
			VictoryFont,
			1.0f,
			false);
	}
}
