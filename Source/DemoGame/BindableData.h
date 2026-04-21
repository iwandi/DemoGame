// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BindableData.generated.h"

class UWidget;

UINTERFACE(BlueprintType)
class UDisplayDataWidget : public UInterface
{
	GENERATED_BODY()
};

class IDisplayDataWidget
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowData(const UObject* data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearData();
};

/**
 * 
 */
UCLASS()
class DEMOGAME_API UBindableData : public UObject
{
	GENERATED_BODY()

	TWeakObjectPtr<UObject> Data;
	TArray<TWeakObjectPtr<UObject>> Bindings;

public :
	void SetData(const TWeakObjectPtr<UObject> data) 
	{
		Data = data;
		DataChanged();
	}

	UFUNCTION(BlueprintCallable)
	void BindDisplay(UObject* display);

	UFUNCTION(BlueprintCallable)
	void DataChanged();
};


/*UCLASS()
template<typename T>
class DEMOGAME_API UDisplayDataWidgetList : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UWidget> WidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<UWidget>> WidgetInstances;
	UPROPERTY()
	TObjectPtr<UWidget> Container;
public:
	UFUNCTION(BlueprintCallable)
	void ShowData(const TArray<T>* data);
	UFUNCTION(BlueprintCallable)
	void ClearData();
};*/