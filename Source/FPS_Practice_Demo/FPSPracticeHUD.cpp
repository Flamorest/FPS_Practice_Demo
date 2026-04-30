// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPracticeHUD.h"
#include "FPS_Practice_DemoGameMode.h"
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

	AFPS_Practice_DemoGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>() : nullptr;

	UFont* ScoreFont = GEngine ? GEngine->GetSmallFont() : nullptr;
	UFont* CrosshairFont = GEngine ? GEngine->GetMediumFont() : nullptr;
	UFont* VictoryFont = GEngine ? GEngine->GetLargeFont() : nullptr;

	if (GameMode)
	{
		const FString ScoreText = FString::Printf(TEXT("Score: %d / %d"), GameMode->GetCurrentScore(), GameMode->GetTargetScoreToWin());
		const FString HitText = FString::Printf(TEXT("Targets Hit: %d"), GameMode->GetHitTargetCount());
		const FString PromptText = GameMode->HasWonGame() ? TEXT("Victory! Press R to restart.") : TEXT("Shoot targets to score.");

		DrawText(ScoreText, FLinearColor::White, 40.0f, 40.0f, ScoreFont, 1.0f, false);
		DrawText(HitText, FLinearColor::White, 40.0f, 65.0f, ScoreFont, 1.0f, false);
		DrawText(PromptText, FLinearColor::White, 40.0f, 90.0f, ScoreFont, 1.0f, false);
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

	if (GameMode && GameMode->HasWonGame())
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
