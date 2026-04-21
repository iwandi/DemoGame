// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableSubsystem.h"
#include "SaveStateSubsystem.h"
#include "GameLivecycleSubsystem.h"
#include "BindableData.h"
#include "DemoGameSaveGame.h"

void UCollectableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SaveStateSubsystem = Cast<USaveStateSubsystem>(Collection.InitializeDependency(USaveStateSubsystem::StaticClass()));
	SaveStateSubsystem->RegisterSaveStateHandler(this);
	GameLivecycleSubsystem = Cast<UGameLivecycleSubsystem>(Collection.InitializeDependency(UGameLivecycleSubsystem::StaticClass()));

	Bindable = NewObject<UBindableData>();
	Bindable->SetData(this);
}

void UCollectableSubsystem::AddCollectable(int64 value)
{
	TotalCollected += value;
	Bindable->DataChanged();

	if (TotalCollected >= TargetValue)
		GameLivecycleSubsystem->GoalReached();
	else
		// Only invalidate save if we are not in the end condition
		// Making it possible to load into the end condition is not smart
		SaveStateSubsystem->InvalidateSaveState();	
}

bool UCollectableSubsystem::IsSaveRequired_Implementation()
{
	// We invalidate SaveStateSubsystem so no need to invalidate this way
	return false;
}

void UCollectableSubsystem::ApplySaveGame_Implementation(UDemoGameSaveGame* saveGame)
{
	TotalCollected = saveGame->TotalCollected;
	Bindable->DataChanged();
}

void UCollectableSubsystem::WriteSaveState_Implementation(UDemoGameSaveGame* saveGame)
{
	saveGame->TotalCollected = TotalCollected;
}

