// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Heart : ModuleRules
{
	public Heart(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"DeveloperSettings",
				"GameplayTags",
				"InputCore",
				"NetCore",
				"StructUtils"
			});

        // Plugin dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"HeartCore"
			});

		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"CoreUObject",
				"Engine",
				"AssetRegistry"
			});
	}
}
