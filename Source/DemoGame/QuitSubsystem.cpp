// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitSubsystem.h"
#include "SaveStateSubsystem.h"
#include "PopupSubsystem.h"

void UQuitSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SaveStateSubsystem = GetGameInstance()->GetSubsystem<USaveStateSubsystem>();
}


void UQuitSubsystem::HandleUserActionQuit(ULocalPlayer* LocalPlayer)
{
	auto quitRequest = NewObject<UPopupRequest>();
	quitRequest->Intent = EPopupIntent::NegativeAction;
	quitRequest->Title = FText::FromString(TEXT("Quit Game"));
	quitRequest->Message = FText::FromString(TEXT("Are you sure you want to quit the game?"));
	quitRequest->Confirmed = FText::FromString(TEXT("Yes"));
	quitRequest->Declined = FText::FromString(TEXT("No"));

	quitRequest->Callback.AddDynamic(this, &UQuitSubsystem::HandleQuitRequest);

	auto popupSubsystem = LocalPlayer->GetSubsystem<UPopupSubsystem>();

	LastUsedPopupSubsystem = popupSubsystem;

	popupSubsystem->ShowPopup(quitRequest);
}

void UQuitSubsystem::HandleQuitRequest(EPopupOption Result)
{
	if (Result == EPopupOption::Confirmed)
	{
		if (SaveStateSubsystem->IsSaveRequired() && IsValid(LastUsedPopupSubsystem))
		{
			auto saveRequest = NewObject<UPopupRequest>();
			saveRequest->Intent = EPopupIntent::NegativeAction;
			saveRequest->Title = FText::FromString(TEXT("Save Game"));
			saveRequest->Message = FText::FromString(TEXT("Do you want to save your progress before quitting?"));
			saveRequest->Confirmed = FText::FromString(TEXT("Yes"));
			saveRequest->Declined = FText::FromString(TEXT("No"));
			saveRequest->Callback.AddDynamic(this, &UQuitSubsystem::HandleSaveRequest);
			LastUsedPopupSubsystem->ShowPopup(saveRequest);
		}
		else
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
}

void UQuitSubsystem::HandleSaveRequest(EPopupOption Result)
{
	if (Result == EPopupOption::Confirmed)
	{
		SaveStateSubsystem->QuickSave();
		FGenericPlatformMisc::RequestExit(false);
	}
	else if (Result == EPopupOption::Declined)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UQuitSubsystem::ForceQuit()
{
	SaveStateSubsystem->EmergencySave();
	FGenericPlatformMisc::RequestExit(false);
}