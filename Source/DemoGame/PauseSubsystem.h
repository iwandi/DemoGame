// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PauseSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPauseReason : uint8
{
	Unknown			UMETA(DisplayName = "Unknown"),
	GameLivecycle	UMETA(DisplayName = "Game Livecycle"),
	UISubsystem		UMETA(DisplayName = "UI Subsystem"),
};

/**
 * 
 */
UCLASS()
class DEMOGAME_API UPauseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private :
	// TODO : This should just be flags on a single field
	TSet<EPauseReason> Reasons;

public:
	UFUNCTION(BlueprintCallable)
	void ResetPause();

	UFUNCTION(BlueprintCallable)
	void EnterPause(EPauseReason Reason);
	UFUNCTION(BlueprintCallable)
	void LeavePause(EPauseReason Reason);
};
