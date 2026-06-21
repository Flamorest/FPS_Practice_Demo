// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPS_Practice_DemoGameState.generated.h"

class APlayerState;

/**
 *  Replicated global match state for HUD display in network play
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AFPS_Practice_DemoGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(ReplicatedUsing=OnRep_MatchState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 TargetScoreToWin = 100;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	bool bHasWonGame = false;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY(ReplicatedUsing=OnRep_MatchState, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	TObjectPtr<APlayerState> WinningPlayerState = nullptr;

public:

	DECLARE_MULTICAST_DELEGATE(FOnMatchStateUpdated);

	AFPS_Practice_DemoGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateMatchState(int32 NewTargetScoreToWin, bool bNewHasWonGame, APlayerState* NewWinningPlayerState);

	int32 GetTargetScoreToWin() const { return TargetScoreToWin; }
	bool HasWonGame() const { return bHasWonGame; }
	APlayerState* GetWinningPlayerState() const { return WinningPlayerState; }

	FOnMatchStateUpdated OnMatchStateUpdated;
};
