// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DemoGameGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ADemoGameGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> UIRootWidget;
	
	/** Constructor */
	ADemoGameGameMode();

	void StartPlay() override;
};



