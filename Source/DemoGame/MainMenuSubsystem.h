// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PopupSubsystem.h"
#include "MainMenuSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMainMenuActionType : uint8
{
	None			UMETA(DisplayName = "None"),
	ContinueGame	UMETA(DisplayName = "Continue Game"),
	OpenNewGame     UMETA(DisplayName = "Open NewGame Menu"),
	OpenLoadGame    UMETA(DisplayName = "Open LoadGame Menu"),
	OpenSaveGame 	UMETA(DisplayName = "Open SaveGame Menu"),
	OpenSettings 	UMETA(DisplayName = "Open Settings Menu"),
	OpenCredits 	UMETA(DisplayName = "Open Credits Menu"),
	RequestQuit 	UMETA(DisplayName = "Request Quit")
};

class UUISubsystem;
class UPopupSubsystem;
class USaveStateSubsystem;
class UQuitSubsystem;
class UGameLivecycleSubsystem;

/**
 * 
 */
UCLASS()
class DEMOGAME_API UMainMenuSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UUISubsystem> UISubsystem;
	UPROPERTY()
	TObjectPtr<UPopupSubsystem> PopupSubsystem;
	UPROPERTY()
	TObjectPtr<USaveStateSubsystem> SaveStateSubsystem;
	UPROPERTY()
	TObjectPtr<UQuitSubsystem> QuitSubsystem;
	UPROPERTY()
	TObjectPtr<UGameLivecycleSubsystem> GameLivecycleSubsystem;

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void HandleUserAction(const EMainMenuActionType action);
};
