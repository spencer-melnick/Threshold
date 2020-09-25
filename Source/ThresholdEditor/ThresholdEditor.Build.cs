// Copyright (c) 2020 Spencer Melnick

using UnrealBuildTool;

public class ThresholdEditor : ModuleRules
{
    public ThresholdEditor(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Threshold", "Slate", "SlateCore", "EditorStyle", "GameplayTags" });

        PublicIncludePaths.AddRange(new string[] {"Threshold"});
    }
}
