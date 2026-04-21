// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveStateSubsystem.generated.h"

class UBindableData;
class ISaveGameSystem;
class UDemoGameSaveGame;
class UGameLivecycleSubsystem;

USTRUCT(BlueprintType)
struct FSaveMetaData
{
	GENERATED_BODY()
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SaveSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SaveName;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FDateTime SaveDate;
};

UINTERFACE(Blueprintable)
class USaveStateHandler : public UInterface
{
	GENERATED_BODY()
};

class ISaveStateHandler
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsSaveRequired();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplySaveGame(UDemoGameSaveGame* saveGame);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WriteSaveState(UDemoGameSaveGame* saveGame);
};

/**
 * 
 */
UCLASS()
class DEMOGAME_API USaveStateSubsystem : public UGameInstanceSubsystem
{
	const int32 MaxSaveSlots = 20;
	const int32 QuickSaveSlot = MaxSaveSlots + 1;

	GENERATED_BODY()

	UPROPERTY()
	TArray<FSaveMetaData> SaveMetaData;
	UPROPERTY()
	TObjectPtr<UBindableData> Bindable;
	UPROPERTY()
	TArray<TObjectPtr<USaveStateActorComponent>> RegisteredComponents;
	UPROPERTY()
	TArray<TWeakObjectPtr<UObject>> RegisteredHandlers;
	
	ISaveGameSystem* SaveGameSystem;
	UPROPERTY()
	TObjectPtr<UGameLivecycleSubsystem> GameLivecycleSubsystem;

	UPROPERTY()
	UDemoGameSaveGame* CurrentSaveGame;
	bool bInSync;
	bool worldReadyReSpawn;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void SaveInitDone(FPlatformUserId userId, bool sucess);
	FString GetSaveName(int32 slot) const;
	void RetainMetaData(int32 slot, const FString& name, const FDateTime& date);

	void ApplySaveGame(UDemoGameSaveGame* saveGame);
	void ApplyReSpawn(UDemoGameSaveGame* saveGame);
public : 
	UFUNCTION(BlueprintCallable)
	UBindableData* GetBindable() const { return Bindable; }
	UFUNCTION(BlueprintCallable)
	TArray<FSaveMetaData> GetSaveSlots() const { return SaveMetaData; }

	UFUNCTION(BlueprintCallable)
	bool IsSaveRequired() const;

	UFUNCTION(BlueprintCallable)
	void LoadNewGame();

	UFUNCTION(BlueprintCallable)
	void LoadSaveGame(int32 saveSlot);

	UFUNCTION(BlueprintCallable)
	void QuickSave();

	UFUNCTION(BlueprintCallable)
	void StoreSaveGame(int32 saveSlot);

	UFUNCTION(BlueprintCallable)
	void EmergencySave();

	UFUNCTION(BlueprintCallable)
	void OnSaveStateComponentBeginPlay(USaveStateActorComponent* component);
	UFUNCTION(BlueprintCallable)
	void OnSaveStateComponentEndPlay(USaveStateActorComponent* component, bool destoryed);

	UFUNCTION(BlueprintCallable)
	void RegisterSaveStateHandler(UObject* handler);

	UFUNCTION(BlueprintCallable)
	void InvalidateSaveState() { bInSync = false; }
	UFUNCTION(BlueprintCallable)
	bool IsValidSaveSlot(int32 saveSlot);

	void WorldReady();
};
