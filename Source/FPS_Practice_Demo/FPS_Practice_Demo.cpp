// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_Demo.h"
#include "HAL/IConsoleManager.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, FPS_Practice_Demo, "FPS_Practice_Demo" );

DEFINE_LOG_CATEGORY(LogFPS_Practice_Demo)

namespace
{
	static TAutoConsoleVariable<int32> CVarFPSPracticeVerboseLogs(
		TEXT("fpspractice.VerboseLogs"),
		1,
		TEXT("Enable or disable verbose FPS Practice demo log output."));

	static TAutoConsoleVariable<int32> CVarFPSPracticeFireDebugLine(
		TEXT("fpspractice.FireDebugLine"),
		1,
		TEXT("Enable or disable debug fire line rendering for the FPS Practice demo."));
}

bool IsFPSPracticeVerboseLoggingEnabled()
{
	return CVarFPSPracticeVerboseLogs.GetValueOnAnyThread() != 0;
}

bool IsFPSPracticeFireDebugLineEnabled()
{
	return CVarFPSPracticeFireDebugLine.GetValueOnAnyThread() != 0;
}
