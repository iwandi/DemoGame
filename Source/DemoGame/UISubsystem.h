// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "MenuRegistry.h"
#include "UISubsystem.generated.h"

class ULocalPlayer;
class UPopupSubsystem;
class UPauseSubsystem;
class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

DECLARE_LOG_CATEGORY_EXTERN(LogUUISubsystem, Log, All);

UINTERFACE(Blueprintable)
class UUIRoot : public UInterface
{
	GENERATED_BODY()
};

class IUIRoot
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UMenuRegistry* GetMenuregistry() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCommonActivatableWidgetStack* GetMenuStack(const EMenuContextType context) const;
};


/**
 * 
 */
UCLASS()
class UUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
private :
	UPROPERTY()
	TObjectPtr<UPopupSubsystem> PopupSubsystem;
	UPROPERTY()
	TObjectPtr<UPauseSubsystem> PauseSubsystem;

	UPROPERTY()
	TObjectPtr<UObject> RootUI;
	UPROPERTY()
	TObjectPtr<UMenuRegistry> MenuRegistry;

	EMenuType LastKnownMenu;
	EMenuContextType CurrentContext;

	bool TryGetTopMost(TWeakObjectPtr<UCommonActivatableWidgetStack> stack, UCommonActivatableWidget*& topMost) const;

	bool TryGetMenuEntryByWidget(UCommonActivatableWidget* widget, FMenuEntry& entry) const;
	bool TryGetMenuEntry(const EMenuType menu, FMenuEntry& entry) const;
	bool TryGetStack(const EMenuContextType context, TWeakObjectPtr<UCommonActivatableWidgetStack>& stack) const;

	void ActivateContext(const EMenuContextType context, bool isRoot);
	void OpenRootMenu(const EMenuContextType context);

	void OnMenuDeactivated();
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	void RestoreRootUI(ULocalPlayer* localPlayer, TSubclassOf<UUserWidget> widgetClass);

	UFUNCTION(BlueprintCallable)
	void UserActionBack();

	UFUNCTION(BlueprintCallable)
	void SetMenuState(const EMenuType menu);
	UFUNCTION(BlueprintCallable)
	void OpenMenu(const EMenuType menu);
};
