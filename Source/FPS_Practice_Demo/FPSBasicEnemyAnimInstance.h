// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "FPSBasicEnemyAnimInstance.generated.h"

class AFPSBasicEnemy;

/**
 *  Minimal anim instance that exposes enemy movement and combat state to an AnimBP.
 */
UCLASS()
class FPS_PRACTICE_DEMO_API UFPSBasicEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(BlueprintReadOnly, Category="Enemy Animation")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Enemy Animation")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category="Enemy Animation")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Category="Enemy Animation")
	bool bIsAttacking = false;

	TWeakObjectPtr<AFPSBasicEnemy> CachedEnemy;
	bool bHasLoggedMovementState = false;
	bool bLastLoggedIsMoving = false;
};
