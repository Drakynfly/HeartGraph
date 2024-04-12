// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartNet : ModuleRules
{
    public HeartNet(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        HeartCore.ApplySharedModuleSetup(this, Target);

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "GameplayTags",
                "NetCore",
                "StructUtils"
            }
        );

        // Plugin dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Blood",
                "Heart",
                "HeartCore"
            });

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine"
            }
        );
    }
}