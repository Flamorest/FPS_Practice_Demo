// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPS_Practice_DemoGameState.generated.h"

/**
 *  Replicated score state for HUD display in network play
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AFPS_Practice_DemoGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(ReplicatedUsing=OnRep_ScoreState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 CurrentScore = 0;

	UPROPERTY(ReplicatedUsing=OnRep_ScoreState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 TargetScoreToWin = 100;

	UPROPERTY(ReplicatedUsing=OnRep_ScoreState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 HitTargetCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_ScoreState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 EnemyKillCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_ScoreState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	bool bHasWonGame = false;

	UFUNCTION()
	void OnRep_ScoreState();

public:

	AFPS_Practice_DemoGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateScoreState(int32 NewCurrentScore, int32 NewTargetScoreToWin, int32 NewHitTargetCount, int32 NewEnemyKillCount, bool bNewHasWonGame);

	int32 GetCurrentScore() const { return CurrentScore; }
	int32 GetTargetScoreToWin() const { return TargetScoreToWin; }
	int32 GetHitTargetCount() const { return HitTargetCount; }
	int32 GetEnemyKillCount() const { return EnemyKillCount; }
	bool HasWonGame() const { return bHasWonGame; }
};
