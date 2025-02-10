// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

using UnrealBuildTool;

public class RockModularGameplayAbilitiesEditor : ModuleRules
{
    public RockModularGameplayAbilitiesEditor(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            [
                "Core",
                "RockModularGameplayAbilities",
                "UnrealEd"
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "ToolMenus",
                "AssetDefinition", 
                "PropertyEditor"
            ]
        );
    }
}