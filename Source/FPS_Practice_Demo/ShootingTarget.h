// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingTarget.generated.h"

class UStaticMeshComponent;

/**
 *  Simple shootable target actor for line trace hit testing
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AShootingTarget : public AActor
{
	GENERATED_BODY()

	/** Target mesh that can be assigned in the editor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TargetMesh;

protected:

	/** Score value associated with this target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target")
	int32 TargetScoreValue = 10;

	/** Hide and disable collision after being hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target")
	bool bHideOnHit = true;

	/** Destroy the actor after being hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target")
	bool bDestroyOnHit = false;

public:

	AShootingTarget();

	/** Handles being hit by a shot */
	UFUNCTION(BlueprintCallable, Category="Target")
	void HandleShotHit(AActor* InstigatorActor);

	/** Returns the target mesh */
	UStaticMeshComponent* GetTargetMesh() const { return TargetMesh; }
};
