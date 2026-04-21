// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MenuRegistry.generated.h"

class UCommonActivatableWidget;

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	None        UMETA(DisplayName = "None"),
	HUD			UMETA(DisplayName = "HUD"),
	Pause       UMETA(DisplayName = "Pause Menu"),
	Settings    UMETA(DisplayName = "Settings"),
	Main 		UMETA(DisplayName = "Main"),
	LoadGame 	UMETA(DisplayName = "Load Game"),
	SaveGame 	UMETA(DisplayName = "Save Game"),
	Credits 	UMETA(DisplayName = "Credits")
};

UENUM(BlueprintType)
enum class EMenuContextType : uint8
{
	None			UMETA(DisplayName = "None"),
	InGame			UMETA(DisplayName = "In Game"),
	Menu			UMETA(DisplayName = "Menu"),
	CurrentContext	UMETA(DisplayName = "Current Context")
};

USTRUCT(BlueprintType)
struct FMenuEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMenuContextType MenuContext;
	UPROPERTY(EditAnywhere)
	EMenuType MenuType;
	UPROPERTY(EditAnywhere)
	EMenuType FixedBackMenu;
	UPROPERTY(EditAnywhere)
	bool IsRootMenu;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DEMOGAME_API UMenuRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FMenuEntry> Menus;
};
