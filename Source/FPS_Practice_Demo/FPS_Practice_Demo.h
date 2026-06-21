// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogFPS_Practice_Demo, Log, All);

FPS_PRACTICE_DEMO_API bool IsFPSPracticeVerboseLoggingEnabled();
FPS_PRACTICE_DEMO_API bool IsFPSPracticeFireDebugLineEnabled();

#define FPS_PRACTICE_VERBOSE_LOG(Format, ...) \
	do \
	{ \
		if (IsFPSPracticeVerboseLoggingEnabled()) \
		{ \
			UE_LOG(LogFPS_Practice_Demo, Log, Format, ##__VA_ARGS__); \
		} \
	} while (false)
