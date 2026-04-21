// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLivecycleSubsystem.h"
#include "UISubsystem.h"
#include "PauseSubsystem.h"
#include "PopupSubsystem.h"
#include "SaveStateSubsystem.h"
#include "BindableData.h"
#include "DemoGameGameUserSettings.h"
#include <Kismet/GameplayStatics.h>

const FName StartUpLevelName = FName("StartUp");
const FName GameLevelName = FName("Lvl_ThirdPerson");

void UGameLivecycleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PauseSubsystem = Cast<UPauseSubsystem>(Collection.InitializeDependency(UPauseSubsystem::StaticClass()));
	SaveStateSubsystem = Cast<USaveStateSubsystem>(Collection.InitializeDependency(USaveStateSubsystem::StaticClass()));

	Bindable = NewObject<UBindableData>(this);
	Bindable->SetData(this);

	UserSettings = Cast<UDemoGameGameUserSettings>(GEngine->GetGameUserSettings());
}

void UGameLivecycleSubsystem::LateInit()
{
	auto player = GetWorld()->GetFirstLocalPlayerFromController();

	UISubsystem = player->GetSubsystem<UUISubsystem>();
}

void UGameLivecycleSubsystem::Start()
{
	if (IsStarted)
	{
		PostLoad();
		return;
	}

	IsStarted = true;

	LateInit();

	auto levelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	if (levelName == StartUpLevelName)
	{
		StartFromStartUp();
	}
	else
	{
		StartFromLoadLevel();
	}
}

void UGameLivecycleSubsystem::StartFromStartUp()
{
	EnterMainMenu();
}

void UGameLivecycleSubsystem::PrepareMainMenu()
{
	IsGamePlayWorldRady = false;
	IsPlaying = false;
	PauseSubsystem->EnterPause(EPauseReason::GameLivecycle);
	LoadLevel(StartUpLevelName);
	EnterMainMenu();
}

void UGameLivecycleSubsystem::EnterMainMenu()
{
	UISubsystem->OpenMenu(EMenuType::Main);
}

void UGameLivecycleSubsystem::StartFromLoadLevel()
{
	IsGamePlayWorldRady = true;
	SaveStateSubsystem->LoadNewGame();

	EnterGame();
}

void UGameLivecycleSubsystem::PrepareGame()
{
	LoadLevel(GameLevelName);
	EnterGame();
}

void UGameLivecycleSubsystem::EnterGame()
{
	UISubsystem->OpenMenu(EMenuType::HUD);
	PauseSubsystem->LeavePause(EPauseReason::GameLivecycle);
	IsPlaying = true;
	IsGamePlayWorldRady = true;
}

void UGameLivecycleSubsystem::LoadLevel(const FName& levelName)
{
	auto currentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	if (currentLevelName == levelName)
		return;

	IsGamePlayWorldRady = false;
	UGameplayStatics::OpenLevel(GetWorld(), levelName);
}

void UGameLivecycleSubsystem::PostLoad()
{
	IsGamePlayWorldRady = true;
	SaveStateSubsystem->WorldReady();
}

void UGameLivecycleSubsystem::StartNewGame()
{
	if (!IsPlaying)
	{
		SaveStateSubsystem->LoadNewGame();
		PrepareGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNewGame called while already playing. Ignoring."));
	}
}

bool UGameLivecycleSubsystem::CanContinueGame() const
{
	return UserSettings->LastUsedSaveSlot != -1 &&
		SaveStateSubsystem->IsValidSaveSlot(UserSettings->LastUsedSaveSlot);
}

void UGameLivecycleSubsystem::ContinueGame()
{
	if (CanContinueGame())
		UserLoadGame(UserSettings->LastUsedSaveSlot);
	else
		StartNewGame();
}

void UGameLivecycleSubsystem::UserLoadGame(int32 saveSlot)
{
	if (IsPlaying)
	{
		auto saveRequest = NewObject<UPopupRequest>();
		saveRequest->Intent = EPopupIntent::NegativeAction;
		saveRequest->Title = FText::FromString(TEXT("Load Game"));
		saveRequest->Message = FText::FromString(TEXT("Loading a game will lose all unsaved progress. Do you want to continue?"));
		saveRequest->Confirmed = FText::FromString(TEXT("Yes"));
		saveRequest->Declined = FText::FromString(TEXT("No"));

		saveRequest->Callback.AddDynamic(this, &UGameLivecycleSubsystem::HandleSaveBeforeLoadRequest);

		// TODO : This is a race condition but fine for now
		stagedLoadSlot = saveSlot;
		PopupSubsystem->ShowPopup(saveRequest);
	}
	else
	{
		LoadGame(saveSlot);
	}
}

void UGameLivecycleSubsystem::HandleSaveBeforeLoadRequest(EPopupOption Result)
{
	if (Result == EPopupOption::Confirmed)
	{
		LoadGame(stagedLoadSlot);
	}
}

void UGameLivecycleSubsystem::LoadGame(int32 saveSlot)
{
	IsGamePlayWorldRady = false;
	SaveStateSubsystem->LoadSaveGame(saveSlot);
	if (!IsPlaying)
		PrepareGame();
	else
		EnterGame();
}

void UGameLivecycleSubsystem::EndGame()
{
	if (IsPlaying)
	{
		PrepareMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("QuitGame called while not playing. Ignoring."));
	}
}

void UGameLivecycleSubsystem::SetLastUsedSaveSlot(int32 saveSlot)
{
	if (saveSlot != UserSettings->LastUsedSaveSlot)
	{
		UserSettings->LastUsedSaveSlot = saveSlot;
		UserSettings->SaveSettings();
		Bindable->DataChanged();
	}
}

void UGameLivecycleSubsystem::GoalReached()
{
	if (IsPlaying)
	{
		PrepareMainMenu();
		UISubsystem->OpenMenu(EMenuType::Credits);
	}
}