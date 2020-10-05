// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventorySystem : ModuleRules
{
	public InventorySystem(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "GameplayTags", "Core", "CoreUObject", "Engine"
        });
        
        PublicIncludePaths.AddRange(new string[] {"InventorySystem/Public"} );
        PrivateIncludePaths.AddRange(new string[] {"InventorySystem/Private"} );
    }
}
