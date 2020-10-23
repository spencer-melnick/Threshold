// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThresholdGame : ModuleRules
{
	public ThresholdGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities", "GameplayTags", "GameplayTasks", "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
            "Niagara", "InventorySystem"
        });

        PublicIncludePaths.AddRange(new string[] {"ThresholdGame/Public"});
        PrivateIncludePaths.AddRange(new string[] {"ThresholdGame/Private"});
    }
}
