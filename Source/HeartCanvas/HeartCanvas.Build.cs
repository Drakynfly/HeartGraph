// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartCanvas : ModuleRules
{
    public HeartCanvas(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        HeartCore.ApplySharedModuleSetup(this, Target);

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "InputCore",
                "GameplayTags",
                "UMG"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Blood",
                "HeartCore",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "StructUtils"
            }
        );
    }
}