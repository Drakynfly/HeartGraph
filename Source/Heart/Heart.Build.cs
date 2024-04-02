// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Heart : ModuleRules
{
	public Heart(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		HeartCore.ApplySharedModuleSetup(this, Target);

        // Engine dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"DeveloperSettings",
				"GameplayTags",
				"InputCore",
				"StructUtils"
			});

        // Plugin dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Blood",
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