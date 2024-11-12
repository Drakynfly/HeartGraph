// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartNet : ModuleRules
{
    public HeartNet(ReadOnlyTargetRules Target) : base(Target)
    {
        HeartCore.ApplySharedModuleSetup(this, Target);

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "GameplayTags",
                "NetCore"
            }
        );

        // Plugin dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Blood",
                "Flakes",
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