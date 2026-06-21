// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPracticePlayerState.h"
#include "FPS_Practice_DemoCharacter.h"
#include "Net/UnrealNetwork.h"
#include "FPS_Practice_Demo.h"

AFPSPracticePlayerState::AFPSPracticePlayerState()
{
	bReplicates = true;
}

void AFPSPracticePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPracticePlayerState, PlayerIndex);
	DOREPLIFETIME(AFPSPracticePlayerState, PlayerDisplayName);
	DOREPLIFETIME(AFPSPracticePlayerState, PlayerColor);
	DOREPLIFETIME(AFPSPracticePlayerState, PlayerScore);
	DOREPLIFETIME(AFPSPracticePlayerState, KillCount);
	DOREPLIFETIME(AFPSPracticePlayerState, DeathCount);
	DOREPLIFETIME(AFPSPracticePlayerState, HitTargetCount);
	DOREPLIFETIME(AFPSPracticePlayerState, EnemyKillCount);
}

void AFPSPracticePlayerState::SetPlayerIdentity(int32 NewPlayerIndex, const FString& NewPlayerDisplayName, const FLinearColor& NewPlayerColor)
{
	PlayerIndex = FMath::Max(1, NewPlayerIndex);
	PlayerDisplayName = NewPlayerDisplayName.IsEmpty() ? FString::Printf(TEXT("Player %d"), PlayerIndex) : NewPlayerDisplayName;
	PlayerColor = NewPlayerColor;

	SetPlayerName(PlayerDisplayName);

	if (AFPS_Practice_DemoCharacter* PracticeCharacter = Cast<AFPS_Practice_DemoCharacter>(GetPawn()))
	{
		PracticeCharacter->RefreshPlayerIdentity();
	}
}

void AFPSPracticePlayerState::AddPlayerScore(int32 ScoreAmount)
{
	PlayerScore = FMath::Max(0, PlayerScore + ScoreAmount);
}

void AFPSPracticePlayerState::AddKill()
{
	++KillCount;
}

void AFPSPracticePlayerState::AddDeath()
{
	++DeathCount;
}

void AFPSPracticePlayerState::AddTargetHit()
{
	++HitTargetCount;
}

void AFPSPracticePlayerState::AddEnemyKill()
{
	++EnemyKillCount;
}

void AFPSPracticePlayerState::OnRep_PlayerStats()
{
	FPS_PRACTICE_VERBOSE_LOG(
		TEXT("PlayerState updated: %s Score=%d Kills=%d Deaths=%d TargetHits=%d EnemyKills=%d"),
		*GetPlayerName(),
		PlayerScore,
		KillCount,
		DeathCount,
		HitTargetCount,
		EnemyKillCount);
}

void AFPSPracticePlayerState::OnRep_PlayerIdentity()
{
	SetPlayerName(PlayerDisplayName);

	if (AFPS_Practice_DemoCharacter* PracticeCharacter = Cast<AFPS_Practice_DemoCharacter>(GetPawn()))
	{
		PracticeCharacter->RefreshPlayerIdentity();
	}

	FPS_PRACTICE_VERBOSE_LOG(
		TEXT("Player identity updated: %s Index=%d Color=(R=%.2f,G=%.2f,B=%.2f,A=%.2f)"),
		*PlayerDisplayName,
		PlayerIndex,
		PlayerColor.R,
		PlayerColor.G,
		PlayerColor.B,
		PlayerColor.A);
}
