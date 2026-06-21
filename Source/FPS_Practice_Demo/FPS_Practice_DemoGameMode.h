// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPS_Practice_DemoGameMode.generated.h"

class AFPSPracticePlayerState;
class AController;
class APlayerState;

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class AFPS_Practice_DemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	/** Score required to trigger victory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Score")
	int32 TargetScoreToWin = 100;

	/** Score awarded when one player kills another player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Score")
	int32 PlayerKillScore = 50;

	/** True once the player has reached the target score */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	bool bHasWonGame = false;

	/** The player who reached the win condition */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	TObjectPtr<APlayerState> WinningPlayerState = nullptr;

public:
	AFPS_Practice_DemoGameMode();

	virtual void InitGameState() override;

	/** Adds score to a specific player and checks the win condition */
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScoreForPlayer(AController* ScoringController, int32 ScoreAmount, AActor* ScoredTarget);

	/** Adds score for a scoring actor by resolving its owning controller */
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScoreForActor(AActor* ScoringActor, int32 ScoreAmount, AActor* ScoredTarget);

	/** Awards score for a player-vs-player kill */
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddPlayerKillScore(AController* KillerController, AController* VictimController);

	/** Returns the score required to win */
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetTargetScoreToWin() const { return TargetScoreToWin; }

	/** Returns true if the player has reached the win condition */
	UFUNCTION(BlueprintPure, Category="Score")
	bool HasWonGame() const { return bHasWonGame; }

protected:

	void SyncGameState() const;
	AFPSPracticePlayerState* GetPracticePlayerState(AController* Controller) const;
	void CheckVictory(AFPSPracticePlayerState* ScoringPlayerState);
};



