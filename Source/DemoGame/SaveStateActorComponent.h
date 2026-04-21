// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveStateActorComponent.generated.h"

class UDemoGameSaveGame;

UENUM(BlueprintType, meta = (Bitflags))
enum class EPresitenceActions : uint8
{
	None		= 0			UMETA(DisplayName = "None"),
	Positon		= 1 << 0	UMETA(DisplayName = "Positon"),
	Rotation	= 1 << 1	UMETA(DisplayName = "Rotation"),
	ReSpawn 	= 1 << 2	UMETA(DisplayName = "ReSpawn")
};
ENUM_CLASS_FLAGS(EPresitenceActions);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMOGAME_API USaveStateActorComponent : public UActorComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	void PostEditImport() override;
#endif

public:	
	USaveStateActorComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid SaveId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/DemoGame.EPresitenceActions"))
	uint8 PresitenceActions;
	EPresitenceActions GetPresitenceActions() const { return static_cast<EPresitenceActions>(PresitenceActions >> 1); }

	UFUNCTION(BlueprintCallable)
	virtual void ApplySaveState(UDemoGameSaveGame* saveGame, bool alreadyPresent, bool reSpawn);

	UFUNCTION(BlueprintCallable)
	virtual bool WriteSaveState(UDemoGameSaveGame* saveGame);

	UFUNCTION(BlueprintCallable)
	virtual void InvalidateSaveState();
};
