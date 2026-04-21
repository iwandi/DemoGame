// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuSubsystem.h"
#include "UISubsystem.h"
#include "PopupSubsystem.h"
#include "SaveStateSubsystem.h"
#include "QuitSubsystem.h"
#include "GameLivecycleSubsystem.h"

void UMainMenuSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UISubsystem = Cast<UUISubsystem>(Collection.InitializeDependency(UUISubsystem::StaticClass()));
	PopupSubsystem = Cast<UPopupSubsystem>(Collection.InitializeDependency(UPopupSubsystem::StaticClass()));

	auto gameInstance = GetLocalPlayer()->GetGameInstance();

	SaveStateSubsystem = gameInstance->GetSubsystem<USaveStateSubsystem>();
	QuitSubsystem = gameInstance->GetSubsystem<UQuitSubsystem>();
	GameLivecycleSubsystem = gameInstance->GetSubsystem<UGameLivecycleSubsystem>();
}

void UMainMenuSubsystem::HandleUserAction(const EMainMenuActionType action)
{
	if (action == EMainMenuActionType::None)
		return;

	if (action == EMainMenuActionType::RequestQuit)
	{
		QuitSubsystem->HandleUserActionQuit(GetLocalPlayer());
	}
	else if (action == EMainMenuActionType::ContinueGame)
	{
		GameLivecycleSubsystem->ContinueGame();
	}
	else if (action == EMainMenuActionType::OpenNewGame)
	{
		GameLivecycleSubsystem->StartNewGame();
	}
	else
	{
		auto menuToOpen = EMenuType::None;
		switch (action)
		{
		case EMainMenuActionType::OpenNewGame:
			menuToOpen = EMenuType::Main;
			break;
		case EMainMenuActionType::OpenLoadGame:
			menuToOpen = EMenuType::LoadGame;
			break;
		case EMainMenuActionType::OpenSaveGame:
			menuToOpen = EMenuType::SaveGame;
			break;
		case EMainMenuActionType::OpenSettings:
			menuToOpen = EMenuType::Settings;
			break;
		case EMainMenuActionType::OpenCredits:
			menuToOpen = EMenuType::Credits;
			break;
		default:
			break;
		}
		if (menuToOpen != EMenuType::None)
		{
			UISubsystem->OpenMenu(menuToOpen);
		}
	}
}

