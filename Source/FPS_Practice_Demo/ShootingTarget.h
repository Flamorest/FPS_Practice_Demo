// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FPSDamageableInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingTarget.generated.h"

class UStaticMeshComponent;

/**
 *  Simple shootable target actor for line trace hit testing
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AShootingTarget : public AActor, public IFPSDamageableInterface
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

	/** True once this target has been hit and processed */
	bool bIsDead = false;

public:

	AShootingTarget();

	/** Handles being hit by a shot */
	UFUNCTION(BlueprintCallable, Category="Target")
	void HandleShotHit(AActor* InstigatorActor);

	/** Handles generic FPS damage */
	virtual void ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	/** Returns true if this target has already been hit */
	virtual bool IsDead_Implementation() const override;

	/** Returns the target mesh */
	UStaticMeshComponent* GetTargetMesh() const { return TargetMesh; }
};
