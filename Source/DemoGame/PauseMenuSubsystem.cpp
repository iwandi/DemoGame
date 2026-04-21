// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuSubsystem.h"
#include "UISubsystem.h"
#include "PopupSubsystem.h"
#include "QuitSubsystem.h"
#include "GameLivecycleSubsystem.h"

void UPauseMenuSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UISubsystem = Cast<UUISubsystem>(Collection.InitializeDependency(UUISubsystem::StaticClass()));
	PopupSubsystem = Cast<UPopupSubsystem>(Collection.InitializeDependency(UPopupSubsystem::StaticClass()));

	auto gameInstance = GetLocalPlayer()->GetGameInstance();

	QuitSubsystem = gameInstance->GetSubsystem<UQuitSubsystem>();
	GameLivecycleSubsystem = gameInstance->GetSubsystem<UGameLivecycleSubsystem>();
}

void UPauseMenuSubsystem::HandleEndGameRequest(EPopupOption Result)
{
	if (Result == EPopupOption::Confirmed)
	{
		GameLivecycleSubsystem->EndGame();
	}
}

void UPauseMenuSubsystem::HandleUserAction(const EPauseMenuActionType action)
{
	if (action == EPauseMenuActionType::None)
		return;

	if (action == EPauseMenuActionType::RequestQuit)
	{
		QuitSubsystem->HandleUserActionQuit(GetLocalPlayer());
	}
	else if (action == EPauseMenuActionType::RequestEndGame)
	{
		auto endGameRequest = NewObject<UPopupRequest>();

		endGameRequest->Intent = EPopupIntent::NegativeAction;
		endGameRequest->Title = FText::FromString(TEXT("End Game"));
		endGameRequest->Message = FText::FromString(TEXT("Are you sure you want to end the current game? Unsaved progress will be lost."));
		endGameRequest->Confirmed = FText::FromString(TEXT("Yes"));
		endGameRequest->Declined = FText::FromString(TEXT("No"));
		endGameRequest->Callback.AddDynamic(this, &UPauseMenuSubsystem::HandleEndGameRequest);

		PopupSubsystem->ShowPopup(endGameRequest);
	}
	else if (action == EPauseMenuActionType::ContinueGame)
	{
		UISubsystem->UserActionBack();
	}
	else
	{
		auto menuToOpen = EMenuType::None;
		switch (action)
		{
		case EPauseMenuActionType::OpenLoadGame:
			menuToOpen = EMenuType::LoadGame;
			break;
		case EPauseMenuActionType::OpenSaveGame:
			menuToOpen = EMenuType::SaveGame;
			break;
		case EPauseMenuActionType::OpenSettings:
			menuToOpen = EMenuType::Settings;
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