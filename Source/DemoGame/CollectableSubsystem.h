// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveStateSubsystem.h"
#include "CollectableSubsystem.generated.h"

class UBindableData;
class UGameLivecycleSubsystem;

/**
 * 
 */
UCLASS()
class DEMOGAME_API UCollectableSubsystem : public UGameInstanceSubsystem, public ISaveStateHandler
{
	const int64 TargetValue = 100.f;

	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USaveStateSubsystem> SaveStateSubsystem;
	UPROPERTY()
	TObjectPtr<UGameLivecycleSubsystem> GameLivecycleSubsystem;
	UPROPERTY()
	TObjectPtr<UBindableData> Bindable;

	int64 TotalCollected = 0.f;

	void Initialize(FSubsystemCollectionBase& Collection) override;
public :
	UFUNCTION(BlueprintCallable)
	UBindableData* GetBindable() const { return Bindable; }

	UFUNCTION(BlueprintCallable)
	int64 GetTotalCollected() const { return TotalCollected; }
	UFUNCTION(BlueprintCallable)
	int64 GetTargetValue() const { return TargetValue; }
	UFUNCTION(BlueprintCallable)
	float GetProgess() const { return static_cast<float>(TotalCollected) / static_cast<float>(TargetValue); }

	UFUNCTION(BlueprintCallable)
	void AddCollectable(int64 value);

	// ISaveStateHandler
	virtual bool IsSaveRequired_Implementation() override;
	virtual void ApplySaveGame_Implementation(UDemoGameSaveGame* SaveGame) override;
	virtual void WriteSaveState_Implementation(UDemoGameSaveGame* SaveGame) override;
};
