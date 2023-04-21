// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartCoreEditor : ModuleRules
{
    public HeartCoreEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "HeartCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "AssetDefinition",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "UnrealEd"
            }
        );
    }
}