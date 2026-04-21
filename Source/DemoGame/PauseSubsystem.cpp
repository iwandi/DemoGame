// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UPauseSubsystem::ResetPause()
{
	Reasons.Empty();
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void UPauseSubsystem::EnterPause(EPauseReason Reason)
{
	if (!Reasons.Contains(Reason))
		Reasons.Add(Reason);

	auto pause = Reasons.Num() > 0;
	UGameplayStatics::SetGamePaused(GetWorld(), pause);
}

void UPauseSubsystem::LeavePause(EPauseReason Reason)
{
	if (Reasons.Contains(Reason))
		Reasons.Remove(Reason);

	auto pause = Reasons.Num() > 0;
	UGameplayStatics::SetGamePaused(GetWorld(), pause);
}