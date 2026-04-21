// Fill out your copyright notice in the Description page of Project Settings.


#include "UISubsystem.h"
#include "CommonActivatableWidget.h"
#include "PopupSubsystem.h"
#include "PauseSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

DEFINE_LOG_CATEGORY(LogUUISubsystem);

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PopupSubsystem = Cast<UPopupSubsystem>(Collection.InitializeDependency(UPopupSubsystem::StaticClass()));

	auto gameInstance = GetLocalPlayer()->GetGameInstance();

	PauseSubsystem = gameInstance->GetSubsystem<UPauseSubsystem>();
}

void UUISubsystem::RestoreRootUI(ULocalPlayer* localPlayer, TSubclassOf<UUserWidget> widgetCalls)
{
	// TODO : This is called by GameMode but should be called back in player Controller on a LoadMap Event

	// We may just get destoryed and need to restore
	// IF we find a valid entry we will atempt to restore
	EMenuType retoreMenu;
	bool restoreEntry = false;
	if (IsValid(RootUI))
	{
		// Check if we can detect the current menu from the stack
		if (CurrentContext != EMenuContextType::None)
		{
			TWeakObjectPtr<UCommonActivatableWidgetStack> stack;
			if (TryGetStack(CurrentContext, stack))
			{
				if(stack.IsValid() && stack->GetNumWidgets() > 0)
				{
					UCommonActivatableWidget* topWidget = nullptr;
					if (TryGetTopMost(stack, topWidget))
					{
						FMenuEntry entry;
						if (TryGetMenuEntryByWidget(topWidget, entry))
						{
							retoreMenu = entry.MenuType;
							restoreEntry = true;
						}
					}
				}
			}
		}

		if (!restoreEntry && LastKnownMenu != EMenuType::None)
		{
			retoreMenu = LastKnownMenu;
			restoreEntry = true;
		}

		// TODO : This may jsut be stright up wrong. 
		// i wantet to remove the widget from its container. This looks not liek the call
		// keeping this for now as it dose not couse a major issue
		RootUI->RemoveFromRoot();
		RootUI = nullptr;
	}

	if (!widgetCalls->ImplementsInterface(UUIRoot::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("UIRootWidget %s does not implement the IUIRoot interface. Please fix this in order to use it as the root widget for the UI subsystem."), *GetNameSafe(widgetCalls));
		return;
	}

	auto playerController = localPlayer->GetPlayerController(GetWorld());
	auto root = CreateWidget<UUserWidget>(playerController, widgetCalls);
	root->AddToPlayerScreen(0);

	RootUI = root;

	auto registry = IUIRoot::Execute_GetMenuregistry(RootUI);

	checkf(
		registry,
		TEXT("UI Root does not have a menu registry. Please fix this to use the menu system.")
	);

	MenuRegistry = registry;

	if (restoreEntry)
	{
		SetMenuState(retoreMenu);
	}
}

bool UUISubsystem::TryGetTopMost(TWeakObjectPtr<UCommonActivatableWidgetStack> stack, UCommonActivatableWidget*& topMost) const
{
	auto list = stack->GetWidgetList();

	// Only Care for the IsActivated on the first element as the others will be inactive
	// But on a pending deactivated top stack element it will already be inactive so we can proced to the next one down and accept it in deactivated state
	bool isFirst = true;
	for (int32 i = list.Num(); i-- > 0; )
	{
		auto widget = list[i];
		if (IsValid(widget) && (!isFirst || widget->IsActivated()))
		{
			topMost = widget;
			return true;
		}
		isFirst = false;
	}
	return false;
}

bool UUISubsystem::TryGetMenuEntryByWidget(UCommonActivatableWidget* widget, FMenuEntry& entry) const
{
	const FMenuEntry* Found = MenuRegistry->Menus.FindByPredicate([widget](const FMenuEntry& Entry)
		{
			return Entry.WidgetClass == widget->GetClass();
		});
	if (Found)
	{
		entry = *Found;
		return true;
	}
	return false;
}

bool UUISubsystem::TryGetMenuEntry(const EMenuType menu, FMenuEntry& entry) const
{
	const FMenuEntry* Found = MenuRegistry->Menus.FindByPredicate([menu](const FMenuEntry& Entry)
		{
			return Entry.MenuType == menu;
		});

	if (Found)
	{
		entry = *Found;
		return true;
	}
	return false;
}

bool UUISubsystem::TryGetStack(const EMenuContextType context, TWeakObjectPtr<UCommonActivatableWidgetStack>& stack) const
{
	auto ret = IUIRoot::Execute_GetMenuStack(RootUI, context);
	stack = ret;
	return IsValid(ret);
}

void UUISubsystem::UserActionBack()
{
	// TODO : Handle Pause

	if (CurrentContext == EMenuContextType::None)
		return;

	// If there is a popup open call it instead
	if (PopupSubsystem->IsPopupOpen())
	{
		PopupSubsystem->UserActionBack();
		return;
	}

	TWeakObjectPtr<UCommonActivatableWidgetStack> stack;
	if (!TryGetStack(CurrentContext, stack))
	{
		UE_LOG(LogUUISubsystem, Warning, TEXT("Menu context %d not found in registry"), (int)CurrentContext);
		return;
	}

	if (stack->GetNumWidgets() > 0)
	{
		auto topWidget = stack->GetActiveWidget();

		FMenuEntry entry;
		if (!TryGetMenuEntryByWidget(topWidget, entry))
		{
			UE_LOG(LogUUISubsystem, Warning, TEXT("Menu %s not found in registry"), *GetNameSafe(topWidget));
			topWidget->DeactivateWidget();

			LastKnownMenu = EMenuType::None;
		}
		else if (entry.IsRootMenu)
		{
			// We are at a root menu noting more to do
			ActivateContext(entry.MenuContext, entry.IsRootMenu);
			LastKnownMenu = entry.MenuType;
		}
		else if (entry.FixedBackMenu != EMenuType::None)
		{
			topWidget->DeactivateWidget();
			OpenMenu(entry.FixedBackMenu);
		}
		else
		{
			topWidget->DeactivateWidget();

			if (TryGetTopMost(stack, topWidget) && IsValid(topWidget) && TryGetMenuEntryByWidget(topWidget, entry))
			{
				LastKnownMenu = entry.MenuType;
				ActivateContext(entry.MenuContext, entry.IsRootMenu);
			}
			else if (stack->GetNumWidgets() == 0)
			{
				// we are at the bottom so we need to change context or open the root
				UE_LOG(LogUUISubsystem, Error, TEXT("No more widgets in current context %d, going back to root menu"), (int)CurrentContext);
				OpenRootMenu(CurrentContext);
			}
			else
			{
				// We remove a widget from the stack but do not know what menu we are in anymore
				// Lets clear the stack and try to go to root
				UE_LOG(LogUUISubsystem, Warning, TEXT("Unknown menu after back action, clearing stack and going back to root menu"));
				stack->ClearWidgets();
				OpenRootMenu(CurrentContext);
			}
		}
	}
}

void UUISubsystem::OpenRootMenu(const EMenuContextType context)
{
	// loop over all Menu entrys and find the first root for the five context
	for (auto entry : MenuRegistry->Menus)
	{
		if (entry.MenuContext == context && entry.IsRootMenu)
		{
			SetMenuState(entry.MenuType);
			return;
		}
	}
	UE_LOG(LogUUISubsystem, Error, TEXT("No root menu found for context %d"), (int)context);
	//CurrentMenu = EMenuType::None;
}

void UUISubsystem::OpenMenu(EMenuType menu)
{
	// TODO : Animate this i the futre ?
	SetMenuState(menu);
}

void UUISubsystem::SetMenuState(EMenuType menu)
{
	/*if (CurrentMenu == menu)
		return;*/

	FMenuEntry entry;
	if (!TryGetMenuEntry(menu, entry))
	{
		UE_LOG(LogUUISubsystem, Warning, TEXT("Menu %d not found in registry"), (int)menu);
		return;
	}

	auto context = entry.MenuContext;
	// For current context prserve context
	if (context == EMenuContextType::CurrentContext)
		context = CurrentContext;
	// Fall back to menu if still no valid context
	if (context == EMenuContextType::None)
		context = EMenuContextType::Menu;

	TWeakObjectPtr<UCommonActivatableWidgetStack> stack;
	if (!TryGetStack(context, stack))
	{
		UE_LOG(LogUUISubsystem, Warning, TEXT("Menu context %d not found in registry"), (int)entry.MenuContext);
		return;
	}

	bool useExisting = false;
	// Check the Active if its the same menu we just return
	auto stackActiveWidget = stack->GetActiveWidget();
	if (IsValid(stackActiveWidget) && stackActiveWidget->GetClass() == entry.WidgetClass)
	{
		useExisting = true;
	}
	else
	{
		// Scan the stack for the widget if it is already present
		auto list = stack->GetWidgetList();
		int depth = list.Num();
		for (auto widget : list)
		{
			depth--;
			if (IsValid(widget) && widget->GetClass() == entry.WidgetClass)
			{
				useExisting = true;
				break;
			}
		}

		if (useExisting)
		{
			// active widget is not in the list
			// as it is not the right class we need to deactivate as the first thing going down the stack
			if (IsValid(stackActiveWidget))
				stackActiveWidget->DeactivateWidget();
			bool first = true;
			for (int32 i = list.Num(); i-- > 0; )
			{
				auto widget = list[i];
				if (IsValid(widget) && widget->GetClass() == entry.WidgetClass)
				{
					if (depth >= 2)
					{
						// We need to remove and add to force activation
						stack->RemoveWidget(*widget);
						stack->AddWidgetInstance(*widget);
					}
					else
						widget->ActivateWidget();
					break;
				}
				if(depth <= 1)
					widget->DeactivateWidget();
				else
					// We can only deactivate the top most so we need to remove the others
					stack->RemoveWidget(*widget);
				first = false;
			}
		}
	}
	
	if (!useExisting)
	{
		auto widgetUClass = entry.WidgetClass.LoadSynchronous();
		auto widget = stack->AddWidget(widgetUClass);

		widget->OnDeactivated().AddUObject(this, &UUISubsystem::OnMenuDeactivated);
	}

	ActivateContext(entry.MenuContext, entry.IsRootMenu);

	LastKnownMenu = entry.MenuType;
	CurrentContext = context;
	
	//UWidgetBlueprintLibrary::SetUserFocus(GetPlayerController(), widget);
}

void UUISubsystem::OnMenuDeactivated()
{
	if (CurrentContext == EMenuContextType::None)
		return;

	TWeakObjectPtr<UCommonActivatableWidgetStack> stack;
	if (!TryGetStack(CurrentContext, stack))
		return;

	if (stack->GetNumWidgets() <= 0)
		return;

	UCommonActivatableWidget* topWidget = nullptr;
	if (!TryGetTopMost(stack, topWidget))
		return;

	FMenuEntry entry;
	if (!TryGetMenuEntryByWidget(topWidget, entry))
		return;

	// We need to do all this to have a proper activate Context in case we need to unpause
	UUISubsystem::ActivateContext(CurrentContext, entry.IsRootMenu);
}

void UUISubsystem::ActivateContext(const EMenuContextType context, bool isRoot)
{
	if (context != EMenuContextType::CurrentContext)
	{
		auto enumPtr = StaticEnum<EMenuContextType>();

		for (int32 i = 0; i < enumPtr->NumEnums(); ++i)
		{
			if (enumPtr->IsValidEnumValue(i))
			{
				auto value = (EMenuContextType)enumPtr->GetValueByIndex(i);
				auto isActiveContext = value == context;
				TWeakObjectPtr<UCommonActivatableWidgetStack> stack;
				if (TryGetStack(value, stack))
				{
					stack->SetVisibility(isActiveContext ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
				}
			}
		}
	}

	bool pause = context != EMenuContextType::InGame || !isRoot;
	if (pause)
	{
		PauseSubsystem->EnterPause(EPauseReason::UISubsystem);
	}
	else
	{
		PauseSubsystem->LeavePause(EPauseReason::UISubsystem);
	}
}