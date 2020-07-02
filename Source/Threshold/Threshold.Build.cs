// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Threshold : ModuleRules
{
	public Threshold(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
