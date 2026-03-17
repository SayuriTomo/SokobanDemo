// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SokobanDemo : ModuleRules
{
	public SokobanDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			"SokobanDemo",
			"SokobanDemo/Actors",
			"SokobanDemo/CoreManager",
			"SokobanDemo/LevelEditor"
		});
	}
}
