// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeathScreenWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "FPS_Practice_DemoCharacter.h"
#include "FPS_Practice_Demo.h"

UDeathScreenWidget::UDeathScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedRef<SWidget> UDeathScreenWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		FadeBackground = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("FadeBackground"));
		FadeBackground->SetBrushColor(FLinearColor::Black);
		FadeBackground->SetHorizontalAlignment(HAlign_Center);
		FadeBackground->SetVerticalAlignment(VAlign_Center);

		if (UCanvasPanelSlot* BackgroundSlot = RootCanvas->AddChildToCanvas(FadeBackground))
		{
			BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			BackgroundSlot->SetOffsets(FMargin(0.0f));
		}

		CenterBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CenterBox"));
		FadeBackground->SetContent(CenterBox);

		DeathTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DeathTextBlock"));
		DeathTextBlock->SetText(FText::FromString(TEXT("You Died")));
		DeathTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		DeathTextBlock->SetJustification(ETextJustify::Center);
		FSlateFontInfo DeathFont = DeathTextBlock->GetFont();
		DeathFont.Size = DeathTextFontSize;
		DeathTextBlock->SetFont(DeathFont);

		if (UVerticalBoxSlot* DeathTextSlot = CenterBox->AddChildToVerticalBox(DeathTextBlock))
		{
			DeathTextSlot->SetHorizontalAlignment(HAlign_Center);
			DeathTextSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 28.0f));
		}

		ReplayButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ReplayButton"));
		ReplayButton->SetBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f));

		ReplayButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ReplayButtonText"));
		ReplayButtonText->SetText(FText::FromString(TEXT("Replay")));
		ReplayButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		ReplayButtonText->SetJustification(ETextJustify::Center);
		FSlateFontInfo ReplayFont = ReplayButtonText->GetFont();
		ReplayFont.Size = ReplayButtonFontSize;
		ReplayButtonText->SetFont(ReplayFont);
		ReplayButton->AddChild(ReplayButtonText);

		if (UVerticalBoxSlot* ReplayButtonSlot = CenterBox->AddChildToVerticalBox(ReplayButton))
		{
			ReplayButtonSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}

	return Super::RebuildWidget();
}

void UDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReplayButton)
	{
		ReplayButton->OnClicked.RemoveDynamic(this, &UDeathScreenWidget::HandleReplayButtonClicked);
		ReplayButton->OnClicked.AddDynamic(this, &UDeathScreenWidget::HandleReplayButtonClicked);
	}
}

void UDeathScreenWidget::NativeDestruct()
{
	if (ReplayButton)
	{
		ReplayButton->OnClicked.RemoveDynamic(this, &UDeathScreenWidget::HandleReplayButtonClicked);
	}

	Super::NativeDestruct();
}

void UDeathScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsFadingIn)
	{
		return;
	}

	CurrentFadeAlpha = FMath::Min(CurrentFadeAlpha + (FadeSpeed * InDeltaTime), TargetFadeAlpha);
	SetRenderOpacity(CurrentFadeAlpha);

	if (CurrentFadeAlpha >= TargetFadeAlpha)
	{
		bIsFadingIn = false;
	}
}

void UDeathScreenWidget::InitializeForCharacter(AFPS_Practice_DemoCharacter* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
}

void UDeathScreenWidget::StartFadeIn()
{
	CurrentFadeAlpha = 0.0f;
	bIsFadingIn = true;
	SetRenderOpacity(0.0f);
	SetReplayEnabled(true);
}

void UDeathScreenWidget::SetReplayEnabled(bool bEnabled)
{
	if (ReplayButton)
	{
		ReplayButton->SetIsEnabled(bEnabled);
	}
}

void UDeathScreenWidget::HandleReplayButtonClicked()
{
	FPS_PRACTICE_VERBOSE_LOG(TEXT("Replay button clicked"));
	SetReplayEnabled(false);

	if (AFPS_Practice_DemoCharacter* Character = OwningCharacter.Get())
	{
		Character->HandleReplayButtonClicked();
	}
}
