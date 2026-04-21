// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PopupSubsystem.h"
#include "PauseMenuSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPauseMenuActionType : uint8
{
	None			UMETA(DisplayName = "None"),
	ContinueGame	UMETA(DisplayName = "Continue Game"),
	OpenLoadGame    UMETA(DisplayName = "Open LoadGame Menu"),
	OpenSaveGame 	UMETA(DisplayName = "Open SaveGame Menu"),
	OpenSettings 	UMETA(DisplayName = "Open Settings Menu"),
	RequestEndGame	UMETA(DisplayName = "Request End Game"),
	RequestQuit 	UMETA(DisplayName = "Request Quit")
};

class UUISubsystem;
class UPopupSubsystem;
class UQuitSubsystem;
class UGameLivecycleSubsystem;

/**
 * 
 */
UCLASS()
class DEMOGAME_API UPauseMenuSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UUISubsystem> UISubsystem;
	UPROPERTY()
	TObjectPtr<UPopupSubsystem> PopupSubsystem;
	UPROPERTY()
	TObjectPtr<UQuitSubsystem> QuitSubsystem;
	UPROPERTY()
	TObjectPtr<UGameLivecycleSubsystem> GameLivecycleSubsystem;

	UFUNCTION(BlueprintCallable)
	void HandleEndGameRequest(EPopupOption Result);
private :
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void HandleUserAction(const EPauseMenuActionType action);
};
