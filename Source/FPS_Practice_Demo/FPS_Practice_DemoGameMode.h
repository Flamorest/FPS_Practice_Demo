// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPS_Practice_DemoGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class AFPS_Practice_DemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	/** Current score accumulated from hit targets */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 CurrentScore = 0;

	/** Score required to trigger victory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Score")
	int32 TargetScoreToWin = 100;

	/** Number of targets hit so far */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 HitTargetCount = 0;

public:
	AFPS_Practice_DemoGameMode();

	/** Adds score from a hit target and checks the win condition */
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(int32 ScoreAmount, AActor* ScoredTarget);

	/** Returns the current accumulated score */
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetCurrentScore() const { return CurrentScore; }

	/** Returns the score required to win */
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetTargetScoreToWin() const { return TargetScoreToWin; }

	/** Returns how many targets have been hit */
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetHitTargetCount() const { return HitTargetCount; }

	/** Returns true if the player has reached the win condition */
	UFUNCTION(BlueprintPure, Category="Score")
	bool HasWonGame() const { return CurrentScore >= TargetScoreToWin; }
};



