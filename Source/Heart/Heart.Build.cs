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
				"InputCore",
				"StructUtils"
			});

        // Plugin dependencies
		PublicDependencyModuleNames.AddRange(
			new string[]
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

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new []
				{
					"UnrealEd",
				});
		}
	}
}
