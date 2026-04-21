// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveStateSubsystem.h"
#include "BindableData.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"
#include "Kismet/GameplayStatics.h"
#include "DemoGameSaveGame.h"
#include "SaveStateActorComponent.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "GameLivecycleSubsystem.h"
#include "Engine/World.h"

void USaveStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	GameLivecycleSubsystem = Cast<UGameLivecycleSubsystem>(Collection.InitializeDependency(UGameLivecycleSubsystem::StaticClass()));

	Bindable = NewObject<UBindableData>(this);
	Bindable->SetData(this);

	// TODO : there is no PlayerController yet so we cant resolve to Local Player
	// this needs a later invokation even on SingleUser Platfroms
	//auto localPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	//auto localPlayerPlatfromId = localPlayer->GetPlatformUserId();
	auto localPlayerPlatfromId = FPlatformUserId::CreateFromInternalId(0);

	auto callback = [this](FPlatformUserId Id, bool bSuccess)
	{
		SaveInitDone(Id, bSuccess);
	};

	SaveGameSystem->InitAsync(true, localPlayerPlatfromId, callback);
}

void USaveStateSubsystem::SaveInitDone(FPlatformUserId userId, bool sucess)
{
	TArray<FString> saveNames;
	if (SaveGameSystem->GetSaveGameNames(saveNames, userId))
	{
		for (auto& name : saveNames)
		{
			// TODO : on consoles UGameplayStatics will create some default or setting saveGame this needs to be ignored
			RetainMetaData(-1, name, FDateTime::Now());
		}
	}
	Bindable->DataChanged();
}

FString USaveStateSubsystem::GetSaveName(int32 slot) const
{
	return FString::FromInt(slot);
}

void USaveStateSubsystem::RetainMetaData(int32 slot, const FString& name, const FDateTime& date)
{
	if(slot < 0)
		slot = SaveMetaData.Num();

	FSaveMetaData metaData;
	metaData.SaveSlot = slot;
	metaData.SaveName = name;
	//metaData.SaveDate = FDateTime::Now();

	bool found = false;
	for (auto& data : SaveMetaData)
	{
		if (data.SaveSlot == slot)
		{
			data.SaveSlot = metaData.SaveSlot;
			data.SaveName = metaData.SaveName;
			//data.SaveDate = metaData.SaveDate;
			found = true;
			break;
		}
	}

	if(!found)
		SaveMetaData.Add(metaData);
}

void USaveStateSubsystem::ApplySaveGame(UDemoGameSaveGame* saveGame)
{
	CurrentSaveGame = saveGame;

	for (auto& handler : RegisteredHandlers)
	{
		// TODO : Get rid of null and invalid handlers if the weak ref clear up
		if (handler.IsValid())
			ISaveStateHandler::Execute_ApplySaveGame(handler.Get(), saveGame);
	}

	for (int32 i = RegisteredComponents.Num() - 1; i >= 0; --i)
	{
		auto comp = RegisteredComponents[i];
		if (IsValid(comp))
		{
			comp->ApplySaveState(saveGame, true, false);
		}
	}

	if (GameLivecycleSubsystem->GetIsGamePlayWorldReady())
	{
		ApplyReSpawn(saveGame);
		bInSync = true;
	}
	else 
		// TODO : add a away to be known in sync to set bInSync on a late load
		worldReadyReSpawn = true;

}

void USaveStateSubsystem::ApplyReSpawn(UDemoGameSaveGame* saveGame)
{
	auto world = GetWorld();
	for (auto& reSpawnData : saveGame->ReSpawns)
	{
		auto actorClass = reSpawnData.Value.ActorClass.LoadSynchronous();
		auto transfrom = reSpawnData.Value.Transform;
		auto actor = world->SpawnActor<AActor>(actorClass, transfrom);
		auto prim = actor->FindComponentByClass<UPrimitiveComponent>();
		if (prim && prim->IsSimulatingPhysics())
			prim->SetPhysicsLinearVelocity(reSpawnData.Value.Velocity);

		auto saveComp = actor->FindComponentByClass<USaveStateActorComponent>();
		if (saveComp)
			saveComp->ApplySaveState(saveGame, false, true);

		auto spawnLoc = actor->GetTransform();
	}
}

void USaveStateSubsystem::WorldReady()
{
	if (worldReadyReSpawn && IsValid(CurrentSaveGame))
		ApplyReSpawn(CurrentSaveGame);

	worldReadyReSpawn = false;
}

void USaveStateSubsystem::LoadNewGame()
{
	auto newGame = Cast<UDemoGameSaveGame>(UGameplayStatics::CreateSaveGameObject(UDemoGameSaveGame::StaticClass()));
	ApplySaveGame(newGame);
}

bool USaveStateSubsystem::IsSaveRequired() const
{
	if (bInSync)
		return false;

	for (auto& handler : RegisteredHandlers)
	{
		// TODO : Get rid of null and invalid handlers if the weak ref clear up
		if (handler.IsValid() && ISaveStateHandler::Execute_IsSaveRequired(handler.Get()))
			return true;			
	}
	return bInSync;
}

void USaveStateSubsystem::LoadSaveGame(int32 saveSlot)
{
	if (saveSlot < 0 || saveSlot > MaxSaveSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to save to invalid slot %d"), saveSlot);
		return;
	}

	TArray<uint8> data;
	auto fileName = GetSaveName(saveSlot);
	SaveGameSystem->LoadGame(false, *fileName, 0, data);
	auto saveGame = Cast<UDemoGameSaveGame>(UGameplayStatics::LoadGameFromMemory(data));	
	ApplySaveGame(saveGame);

	GameLivecycleSubsystem->SetLastUsedSaveSlot(saveSlot);
}

void USaveStateSubsystem::StoreSaveGame(int32 saveSlot)
{
	if (saveSlot == -1)
		saveSlot = SaveMetaData.Num();

	if (saveSlot < 0 || saveSlot > MaxSaveSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to save to invalid slot %d"), saveSlot);
		return;
	}

	if (!IsValid(CurrentSaveGame))
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to save while no save game is already loaded. This is not supported yet."));
		return;
	}

	// re-use the CurrentSaveGame as we captured all EndPlay writes in this saveGame
	auto saveGame = CurrentSaveGame;

	for (auto& handler : RegisteredHandlers)
	{
		// TODO : Get rid of null and invalid handlers if the weak ref clear up
		if (handler.IsValid())
			ISaveStateHandler::Execute_WriteSaveState(handler.Get(), saveGame);
	}

	for (auto& comp : RegisteredComponents)
	{
		if (IsValid(comp))
			comp->WriteSaveState(saveGame);
	}

	TArray<uint8> data;
	auto fileName = GetSaveName(saveSlot);
	UGameplayStatics::SaveGameToMemory(saveGame, data);
	SaveGameSystem->SaveGame(false, *fileName, 0, data);
	RetainMetaData(saveSlot, fileName, FDateTime::Now());
	Bindable->DataChanged();

	GameLivecycleSubsystem->SetLastUsedSaveSlot(saveSlot);
}

void USaveStateSubsystem::QuickSave()
{
	if (!IsSaveRequired())
		return;

	StoreSaveGame(QuickSaveSlot);
}

void USaveStateSubsystem::EmergencySave()
{
	UE_LOG(LogTemp, Log, TEXT("EmergencySave called"));
}

void USaveStateSubsystem::OnSaveStateComponentBeginPlay(USaveStateActorComponent* component)
{
	if (IsValid(CurrentSaveGame))
		component->ApplySaveState(CurrentSaveGame, false, false);
	RegisteredComponents.Add(component);
}

void USaveStateSubsystem::OnSaveStateComponentEndPlay(USaveStateActorComponent* component, bool destoryed)
{
	if (!destoryed && IsValid(CurrentSaveGame))
		component->WriteSaveState(CurrentSaveGame);
	if(RegisteredComponents.Contains(component))
		RegisteredComponents.Remove(component);
}

void USaveStateSubsystem::RegisterSaveStateHandler(UObject* handler)
{
	// TODO : Get rid of null and invalid handlers if the weak ref clear up

	bool isValid = IsValid(handler);
	bool isSaveHandler = handler->Implements<USaveStateHandler>();

	if (IsValid(handler) && handler->Implements<USaveStateHandler>())
		RegisteredHandlers.Add(handler);
}

bool USaveStateSubsystem::IsValidSaveSlot(int32 saveSlot)
{
	auto metaData = SaveMetaData.FindByPredicate([saveSlot](const FSaveMetaData& data) 
		{ 
			return data.SaveSlot == saveSlot; 
		});
	return metaData != nullptr;
}