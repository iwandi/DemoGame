// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveStateActorComponent.h"
#include "SaveStateSubsystem.h"
#include "DemoGameSaveGame.h"

#if WITH_EDITOR
void USaveStateActorComponent::PostEditImport()
{
	Super::PostEditImport();
	if (SaveId.IsValid() == false)
	{
		SaveId = FGuid::NewGuid();
	}
}
#endif

USaveStateActorComponent::USaveStateActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveStateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	auto saveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveStateSubsystem>();
	saveSubsystem->OnSaveStateComponentBeginPlay(this);
}

void USaveStateActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	auto saveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveStateSubsystem>();
	saveSubsystem->OnSaveStateComponentEndPlay(this, EndPlayReason == EEndPlayReason::Destroyed);
}

void USaveStateActorComponent::ApplySaveState(UDemoGameSaveGame* saveGame, bool alreadyPresent, bool reSpawn)
{
	auto actions = GetPresitenceActions();

	if ((actions & EPresitenceActions::ReSpawn) == EPresitenceActions::ReSpawn)
	{
		if(alreadyPresent)
			GetOwner()->Destroy();
		else if (reSpawn)
		{
			// Noting to do hier we do not even know the entry we belong to in the save data
		}
	}

	if ((actions & EPresitenceActions::Positon) == EPresitenceActions::Positon ||
		(actions & EPresitenceActions::Rotation) == EPresitenceActions::Rotation)
	{
		if (saveGame->Actors.Num() > 0)
		{
			auto data = saveGame->Actors.Find(SaveId);

			if (data)
			{
				auto transfrom = data->Transform;
				FVector location = GetOwner()->GetActorLocation();
				FRotator rotation = GetOwner()->GetActorRotation();

				if ((actions & EPresitenceActions::Positon) == EPresitenceActions::Positon)
					location = transfrom.GetLocation();
				if ((actions & EPresitenceActions::Rotation) == EPresitenceActions::Rotation)
					rotation = transfrom.Rotator();

				GetOwner()->TeleportTo(location, rotation);
			}
		}
	}
}

bool USaveStateActorComponent::WriteSaveState(UDemoGameSaveGame* saveGame)
{
	auto actions = GetPresitenceActions();

	if ((actions & EPresitenceActions::ReSpawn) == EPresitenceActions::ReSpawn) 
	{
		auto owner = GetOwner();
		auto id = owner->GetUniqueID();
		auto ownerClass = owner->GetClass();

		FReSpawnSaveData data{
			.Transform = owner->GetTransform(),
			.Velocity = owner->GetVelocity(),
			.ActorClass = TSoftClassPtr<AActor>(ownerClass)
		};

		if (saveGame->ReSpawns.Contains(id))
			saveGame->ReSpawns[id] = data;
		else
			saveGame->ReSpawns.Add(id, data);
	}
	else if ((actions & EPresitenceActions::Positon) == EPresitenceActions::Positon ||
		(actions & EPresitenceActions::Rotation) == EPresitenceActions::Rotation)
	{
		auto transfrom = GetOwner()->GetTransform();

		FActorIdSaveData data{
			.Transform = transfrom
		};

		if(saveGame->Actors.Contains(SaveId))
			saveGame->Actors[SaveId] = data;
		else
			saveGame->Actors.Add(SaveId, data);
	}

	return false;
}

void USaveStateActorComponent::InvalidateSaveState()
{
	auto saveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveStateSubsystem>();
	saveSubsystem->InvalidateSaveState();
}