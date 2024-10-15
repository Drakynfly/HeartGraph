// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Heart : ModuleRules
{
	public Heart(ReadOnlyTargetRules Target) : base(Target)
	{
		HeartCore.ApplySharedModuleSetup(this, Target);

        // Engine dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"DeveloperSettings",
				"GameplayTags",
				"InputCore"
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