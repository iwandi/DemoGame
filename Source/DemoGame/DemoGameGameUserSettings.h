// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "DemoGameGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(Config = GameUserSettings)
class DEMOGAME_API UDemoGameGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config)
	int32 LastUsedSaveSlot = -1;
};
