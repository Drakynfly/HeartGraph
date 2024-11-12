// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartJson : ModuleRules
{
    public HeartJson(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "GameplayTags",
                "Flakes",
                "Json",
                "JsonUtilities",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
            }
        );
    }
}