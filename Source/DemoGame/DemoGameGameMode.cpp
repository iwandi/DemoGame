// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoGameGameMode.h"
#include "GameLivecycleSubsystem.h"
#include "UISubsystem.h"

ADemoGameGameMode::ADemoGameGameMode()
{
	// stub
}

void ADemoGameGameMode::StartPlay()
{
	Super::StartPlay();

	auto gameInstance = GetGameInstance();

	auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	auto uiSubsystem = localPlayer->GetSubsystem<UUISubsystem>();
	uiSubsystem->RestoreRootUI(localPlayer, UIRootWidget);

	auto gameLiveCycleSubsystem = gameInstance->GetSubsystem<UGameLivecycleSubsystem>();
	gameLiveCycleSubsystem->Start();
}