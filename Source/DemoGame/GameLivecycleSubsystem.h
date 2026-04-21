// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLivecycleSubsystem.generated.h"

class UUISubsystem;
class UPauseSubsystem;
class UPopupSubsystem;
class USaveStateSubsystem;
class UDemoGameGameUserSettings;

/**
 * 
 */
UCLASS()
class DEMOGAME_API UGameLivecycleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UUISubsystem> UISubsystem;
	UPROPERTY()
	TObjectPtr<UPauseSubsystem> PauseSubsystem;
	UPROPERTY()
	TObjectPtr<UPopupSubsystem> PopupSubsystem;
	UPROPERTY()
	TObjectPtr<USaveStateSubsystem> SaveStateSubsystem;
	UPROPERTY()
	TObjectPtr<UBindableData> Bindable;
	UPROPERTY()
	TObjectPtr<UDemoGameGameUserSettings> UserSettings;

	bool IsStarted;
	bool IsPlaying;
	int32 stagedLoadSlot;
	bool IsGamePlayWorldRady;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LateInit();

	void StartFromStartUp();
	void StartFromLoadLevel();

	void LoadLevel(const FName& levelName);

	void PrepareMainMenu();
	void EnterMainMenu();
	void PrepareGame();
	void EnterGame();

	void PostLoad();
public :
	bool GetIsGamePlayWorldReady() const { return IsGamePlayWorldRady; }

	UFUNCTION(BlueprintCallable)
	UBindableData* GetBindable() const { return Bindable; }
	UFUNCTION(BlueprintCallable)
	bool CanContinueGame() const;
	UFUNCTION(BlueprintCallable)
	void SetLastUsedSaveSlot(int32 saveSlot);

	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	UFUNCTION(BlueprintCallable)
	void ContinueGame();

	UFUNCTION(BlueprintCallable)
	void GoalReached();

	UFUNCTION(BlueprintCallable)
	void UserLoadGame(int32 saveSlot);
	UFUNCTION(BlueprintCallable)
	void HandleSaveBeforeLoadRequest(EPopupOption Result);
	UFUNCTION(BlueprintCallable)
	void LoadGame(int32 saveSlot);

	UFUNCTION(BlueprintCallable)
	void EndGame();
};
