// Copyright (c) 2020 Spencer Melnick

using UnrealBuildTool;

public class ThresholdUI : ModuleRules
{
	public ThresholdUI(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
           "Core", "CoreUObject", "Engine", "InputCore", "UMG", "InventorySystem", "ThresholdGame"
        });
        
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate", "SlateCore"
        });
        
        PublicIncludePaths.AddRange(new string[] {"ThresholdUI/Public"} );
        PrivateIncludePaths.AddRange(new string[] {"ThresholdUI/Private"} );
    }
}
