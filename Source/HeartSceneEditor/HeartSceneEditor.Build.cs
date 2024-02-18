// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartSceneEditor : ModuleRules
{
    public HeartSceneEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        HeartCore.ApplySharedModuleSetup(this, Target);

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "Heart",
                "HeartEditor",
                "HeartScene"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "AdvancedPreviewScene",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "InputCore",
                "Slate",
                "SlateCore",
                "UnrealEd"
            }
        );
    }
}