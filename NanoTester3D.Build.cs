// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NanoTester3D : ModuleRules
{
	public NanoTester3D(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"SlateCore",
			"HTTP", "Json", "JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"NanoTester3D",
			"NanoTester3D/Variant_Platforming",
			"NanoTester3D/Variant_Platforming/Animation",
			"NanoTester3D/Variant_Combat",
			"NanoTester3D/Variant_Combat/AI",
			"NanoTester3D/Variant_Combat/Animation",
			"NanoTester3D/Variant_Combat/Gameplay",
			"NanoTester3D/Variant_Combat/Interfaces",
			"NanoTester3D/Variant_Combat/UI",
			"NanoTester3D/Variant_SideScrolling",
			"NanoTester3D/Variant_SideScrolling/AI",
			"NanoTester3D/Variant_SideScrolling/Gameplay",
			"NanoTester3D/Variant_SideScrolling/Interfaces",
			"NanoTester3D/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
