// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

class AFPS_Practice_DemoCharacter;
class UBorder;
class UButton;
class UCanvasPanel;
class UTextBlock;
class UVerticalBox;

/**
 *  Simple C++ death screen with fade-in and replay button
 */
UCLASS()
class FPS_PRACTICE_DEMO_API UDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UDeathScreenWidget(const FObjectInitializer& ObjectInitializer);

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitializeForCharacter(AFPS_Practice_DemoCharacter* InOwningCharacter);
	void StartFadeIn();
	void SetReplayEnabled(bool bEnabled);

protected:

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> FadeBackground;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> CenterBox;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DeathTextBlock;

	UPROPERTY(Transient)
	TObjectPtr<UButton> ReplayButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ReplayButtonText;

	TWeakObjectPtr<AFPS_Practice_DemoCharacter> OwningCharacter;

	float CurrentFadeAlpha = 0.0f;
	float TargetFadeAlpha = 1.0f;
	float FadeSpeed = 1.5f;
	bool bIsFadingIn = false;

	UFUNCTION()
	void HandleReplayButtonClicked();
};
