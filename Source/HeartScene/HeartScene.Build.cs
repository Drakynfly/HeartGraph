// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartScene : ModuleRules
{
    public HeartScene(ReadOnlyTargetRules Target) : base(Target)
    {
        HeartCore.ApplySharedModuleSetup(this, Target);

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "Heart",
                "HeartCore",
                "EnhancedInput"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "Slate",
                "SlateCore"
            }
        );
    }
}