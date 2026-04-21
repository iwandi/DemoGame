// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DemoGameSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FActorIdSaveData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
};

USTRUCT(BlueprintType)
struct FReSpawnSaveData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AActor> ActorClass;
};

/**
 * 
 */
UCLASS()
class DEMOGAME_API UDemoGameSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 TotalCollected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGuid , FActorIdSaveData> Actors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FReSpawnSaveData> ReSpawns;
};
