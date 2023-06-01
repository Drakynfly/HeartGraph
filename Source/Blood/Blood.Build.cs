// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Blood : ModuleRules
{
    public Blood(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // IDK, something about this module is broken, and if Unity is enabled it blows up :?
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "StructUtils",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}