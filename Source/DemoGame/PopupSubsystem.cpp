// Fill out your copyright notice in the Description page of Project Settings.


#include "PopupSubsystem.h"

bool UPopupSubsystem::IsPopupOpen() const
{
	// TODO : return true if a popup is currently open
	return false;
}

void UPopupSubsystem::ShowPopup(const UPopupRequest* PopupRequest)
{
	// TODO : show popup and call callback

	// For now we just call the callback with a dummy result to test the flow
	PopupRequest->Callback.Broadcast(EPopupOption::Confirmed);
}

void UPopupSubsystem::UserActionBack()
{
	// TODO : Handle back action on popup (if needed)
}