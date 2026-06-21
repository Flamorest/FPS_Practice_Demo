// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSBasicEnemyAnimInstance.h"
#include "FPSBasicEnemy.h"
#include "FPS_Practice_Demo.h"
#include "GameFramework/PawnMovementComponent.h"

void UFPSBasicEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CachedEnemy = Cast<AFPSBasicEnemy>(TryGetPawnOwner());
}

void UFPSBasicEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedEnemy.IsValid())
	{
		CachedEnemy = Cast<AFPSBasicEnemy>(TryGetPawnOwner());
	}

	AFPSBasicEnemy* Enemy = CachedEnemy.Get();
	if (!Enemy)
	{
		Speed = 0.0f;
		bIsMoving = false;
		bIsDead = false;
		bIsAttacking = false;
		return;
	}

	Speed = Enemy->GetVelocity().Size2D();
	bIsMoving = Speed > 3.0f;
	bIsDead = Enemy->IsDeadForAnimation();
	bIsAttacking = Enemy->IsAttackingForAnimation();

	if (!bHasLoggedMovementState || bLastLoggedIsMoving != bIsMoving)
	{
		bHasLoggedMovementState = true;
		bLastLoggedIsMoving = bIsMoving;
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy speed updated: %s Speed=%.1f Moving=%s"), *GetNameSafe(Enemy), Speed, bIsMoving ? TEXT("true") : TEXT("false"));
	}
}
