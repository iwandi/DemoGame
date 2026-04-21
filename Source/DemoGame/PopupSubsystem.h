// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PopupSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPopupIntent : uint8
{
	NeutralAction			UMETA(DisplayName = "Neutral Action"),
	PositiveAction			UMETA(DisplayName = "Positive Action"),
	NegativeAction			UMETA(DisplayName = "Negative Action"),
};

UENUM(BlueprintType, meta = (Bitflags))
enum class EPopupOption : uint8
{
	None		= 0			UMETA(Hidden),
	Confirmed	= 1 << 0	UMETA(DisplayName = "Confirmed"),
	Declined	= 1 << 1	UMETA(DisplayName = "Declined"),
	Canceled	= 1 << 2	UMETA(DisplayName = "Canceled")
};
ENUM_CLASS_FLAGS(EPopupOption);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopupDelegate, EPopupOption, Result);

UCLASS(BlueprintType)
class UPopupRequest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	EPopupIntent Intent;
	UPROPERTY(BlueprintReadWrite)
	EPopupOption Option;

	UPROPERTY(BlueprintReadWrite)
	FText Title;
	UPROPERTY(BlueprintReadWrite)
	FText Message;
	UPROPERTY(BlueprintReadWrite)
	FText Confirmed;
	UPROPERTY(BlueprintReadWrite)
	FText Declined;
	UPROPERTY(BlueprintReadWrite)
	FText Canceled;
	UPROPERTY(BlueprintAssignable)
	FPopupDelegate Callback;
};

/**
 * 
 */
UCLASS()
class DEMOGAME_API UPopupSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	bool IsPopupOpen() const;

	UFUNCTION(BlueprintCallable)
	void ShowPopup(const UPopupRequest* Request);

	UFUNCTION(BlueprintCallable)
	void UserActionBack();
};
