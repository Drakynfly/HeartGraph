// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Heart : ModuleRules
{
	public Heart(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;

        // Engine dependencies
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"InputCore",
				"UMG" // For the node palette widget
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
				"Engine"
			});
	}
}
