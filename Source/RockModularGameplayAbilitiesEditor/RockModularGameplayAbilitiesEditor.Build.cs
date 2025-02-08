// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

using UnrealBuildTool;

public class RockModularGameplayAbilitiesEditor : ModuleRules
{
    public RockModularGameplayAbilitiesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RockModularGameplayAbilities",
                "UnrealEd",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "ToolMenus",
                "AssetDefinition", 
                "PropertyEditor",
            }
        );
    }
}