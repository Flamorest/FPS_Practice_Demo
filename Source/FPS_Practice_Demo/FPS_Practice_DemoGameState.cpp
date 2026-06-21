// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "FPS_Practice_Demo.h"

AFPS_Practice_DemoGameState::AFPS_Practice_DemoGameState()
{
	bReplicates = true;
}

void AFPS_Practice_DemoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_Practice_DemoGameState, TargetScoreToWin);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, bHasWonGame);
	DOREPLIFETIME(AFPS_Practice_DemoGameState, WinningPlayerState);
}

void AFPS_Practice_DemoGameState::UpdateMatchState(int32 NewTargetScoreToWin, bool bNewHasWonGame, APlayerState* NewWinningPlayerState)
{
	TargetScoreToWin = NewTargetScoreToWin;
	bHasWonGame = bNewHasWonGame;
	WinningPlayerState = NewWinningPlayerState;

	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("GameState match updated: TargetScoreToWin %d, Victory %s, Winner=%s"),
		TargetScoreToWin,
		bHasWonGame ? TEXT("true") : TEXT("false"),
		*GetNameSafe(WinningPlayerState.Get()));

	OnMatchStateUpdated.Broadcast();
}

void AFPS_Practice_DemoGameState::OnRep_MatchState()
{
	UE_LOG(
		LogFPS_Practice_Demo,
		Log,
		TEXT("GameState match updated: TargetScoreToWin %d, Victory %s, Winner=%s"),
		TargetScoreToWin,
		bHasWonGame ? TEXT("true") : TEXT("false"),
		*GetNameSafe(WinningPlayerState.Get()));

	if (bHasWonGame)
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Victory replicated"));
	}

	OnMatchStateUpdated.Broadcast();
}
