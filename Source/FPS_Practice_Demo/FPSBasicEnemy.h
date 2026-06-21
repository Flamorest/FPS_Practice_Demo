// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "CoreMinimal.h"
#include "FPSDamageableInterface.h"
#include "GameFramework/Character.h"
#include "FPSBasicEnemy.generated.h"

/**
 *  Minimal enemy that can be damaged and killed by the player's hitscan fire
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AFPSBasicEnemy : public ACharacter, public IFPSDamageableInterface
{
	GENERATED_BODY()

protected:

	/** Radius within which the enemy will detect and chase a player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float DetectionRadius = 2000.0f;

	/** Distance from the player at which movement is considered complete */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float AcceptanceRadius = 150.0f;

	/** How often to refresh the move target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float MoveUpdateInterval = 0.25f;

	/** Range within which the enemy can hit the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackRange = 300.0f;

	/** Damage dealt per attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackDamage = 10.0f;

	/** Minimum time between attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackCooldown = 1.0f;

	/** Optional montage to play when the enemy attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	/** If true, attempt to play AttackMontage when attacking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	bool bUseAttackMontage = true;

	/** Lightweight fallback duration used when no montage is available */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	float AttackFeedbackDuration = 0.25f;

	/** Maximum health for this enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy")
	float MaxHealth = 50.0f;

	/** Current enemy health */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	float CurrentHealth = 50.0f;

	/** Score awarded when this enemy dies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy")
	int32 ScoreValue = 20;

	/** True once this enemy has died */
	UPROPERTY(ReplicatedUsing=OnRep_DeadState, VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	bool bIsDead = false;

	/** True while the enemy is in its attack feedback state */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	bool bIsAttacking = false;

	/** Timer used to refresh simple chase behavior */
	FTimerHandle MoveUpdateTimer;

	/** Timer used to clear temporary attack feedback state */
	FTimerHandle AttackStateTimer;

	/** Game time when the enemy last attacked */
	float LastAttackTime = -1000.0f;

public:

	AFPSBasicEnemy();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Handles generic FPS damage */
	virtual void ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	/** Returns true if this enemy is dead */
	virtual bool IsDead_Implementation() const override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Handles enemy death */
	void Die(AActor* DamageCauser);

	/** Updates chase movement toward the nearest player */
	void UpdateMovementTarget();

	/** Finds the closest valid player pawn */
	APawn* FindNearestPlayerPawn() const;

	/** Attempts a simple melee attack on the target pawn */
	void TryAttackPlayer(APawn* TargetPawn);

	UFUNCTION()
	void OnRep_DeadState();

	void ApplyDeadState();
	void TriggerAttackAnimation();
	void ClearAttackState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttackAnimation();

public:

	bool IsDeadForAnimation() const { return bIsDead; }
	bool IsAttackingForAnimation() const { return bIsAttacking; }
};
