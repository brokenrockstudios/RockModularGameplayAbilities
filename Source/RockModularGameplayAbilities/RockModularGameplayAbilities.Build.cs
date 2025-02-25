// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

using UnrealBuildTool;

public class RockModularGameplayAbilities : ModuleRules
{
	public RockModularGameplayAbilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayAbilities",
				"GameplayMessageRuntime",
				"GameplayTags",
				"GameplayTasks",
				// ... add other public dependencies that you statically link with here ...
				"PhysicsCore",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"UMG",
				"UIExtension",
				"IrisCore", 
				"ModularGameplay",
				"NetCore", 
			}
		);
	}
}