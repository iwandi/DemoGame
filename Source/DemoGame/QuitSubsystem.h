// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <PopupSubsystem.h>
#include "QuitSubsystem.generated.h"

class UPopupSubsystem;
class USaveStateSubsystem;
class ULocalPlayer;

/**
 * 
 */
UCLASS()
class DEMOGAME_API UQuitSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USaveStateSubsystem> SaveStateSubsystem;

	// This is a ungly to retain the last used popupsystem but for now its fine.
	// In a split screen scenario this might couse issues
	UPROPERTY()
	TObjectPtr<UPopupSubsystem> LastUsedPopupSubsystem;

	void Initialize(FSubsystemCollectionBase& Collection);

	UFUNCTION(BlueprintCallable)
	void HandleQuitRequest(EPopupOption Result);
	UFUNCTION(BlueprintCallable)
	void HandleSaveRequest(EPopupOption Result);
public :
	UFUNCTION(BlueprintCallable)
	void HandleUserActionQuit(ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable)
	void ForceQuit();
};
