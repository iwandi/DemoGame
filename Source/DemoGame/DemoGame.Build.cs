// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DemoGame : ModuleRules
{
	public DemoGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"CommonUI",
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"DemoGame",
			"DemoGame/Variant_Platforming",
			"DemoGame/Variant_Platforming/Animation",
			"DemoGame/Variant_Combat",
			"DemoGame/Variant_Combat/AI",
			"DemoGame/Variant_Combat/Animation",
			"DemoGame/Variant_Combat/Gameplay",
			"DemoGame/Variant_Combat/Interfaces",
			"DemoGame/Variant_Combat/UI",
			"DemoGame/Variant_SideScrolling",
			"DemoGame/Variant_SideScrolling/AI",
			"DemoGame/Variant_SideScrolling/Gameplay",
			"DemoGame/Variant_SideScrolling/Interfaces",
			"DemoGame/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
