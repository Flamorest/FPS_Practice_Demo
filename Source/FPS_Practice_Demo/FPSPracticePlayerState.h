// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPracticePlayerState.generated.h"

/**
 *  Replicated per-player score and combat stats
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AFPSPracticePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(ReplicatedUsing=OnRep_PlayerIdentity, VisibleAnywhere, BlueprintReadOnly, Category="Identity")
	int32 PlayerIndex = 1;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerIdentity, VisibleAnywhere, BlueprintReadOnly, Category="Identity")
	FString PlayerDisplayName = TEXT("Player 1");

	UPROPERTY(ReplicatedUsing=OnRep_PlayerIdentity, VisibleAnywhere, BlueprintReadOnly, Category="Identity")
	FLinearColor PlayerColor = FLinearColor::Red;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 PlayerScore = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 KillCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 DeathCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 HitTargetCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats, VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 EnemyKillCount = 0;

	UFUNCTION()
	void OnRep_PlayerStats();

	UFUNCTION()
	void OnRep_PlayerIdentity();

public:

	AFPSPracticePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddPlayerScore(int32 ScoreAmount);
	void AddKill();
	void AddDeath();
	void AddTargetHit();
	void AddEnemyKill();
	void SetPlayerIdentity(int32 NewPlayerIndex, const FString& NewPlayerDisplayName, const FLinearColor& NewPlayerColor);

	int32 GetPlayerIndex() const { return PlayerIndex; }
	const FString& GetPlayerDisplayName() const { return PlayerDisplayName; }
	const FLinearColor& GetPlayerColor() const { return PlayerColor; }
	int32 GetPlayerScore() const { return PlayerScore; }
	int32 GetKillCount() const { return KillCount; }
	int32 GetDeathCount() const { return DeathCount; }
	int32 GetHitTargetCount() const { return HitTargetCount; }
	int32 GetEnemyKillCount() const { return EnemyKillCount; }
};
