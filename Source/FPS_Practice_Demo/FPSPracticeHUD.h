// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSPracticeHUD.generated.h"

/**
 *  Simple HUD that displays score, target hits, a crosshair, and victory text
 */
UCLASS()
class FPS_PRACTICE_DEMO_API AFPSPracticeHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;
};
