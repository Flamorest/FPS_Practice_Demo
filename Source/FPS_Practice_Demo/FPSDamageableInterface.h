// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPSDamageableInterface.generated.h"

UINTERFACE(BlueprintType)
class FPS_PRACTICE_DEMO_API UFPSDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Common interface for actors that can receive FPS-style damage
 */
class FPS_PRACTICE_DEMO_API IFPSDamageableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Damage")
	void ReceiveFPSDamage(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Damage")
	bool IsDead() const;
};
