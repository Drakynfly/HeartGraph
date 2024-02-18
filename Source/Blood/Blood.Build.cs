// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Blood : ModuleRules
{
    public Blood(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        ApplySharedModuleSetup(this, Target);

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

    public static void ApplySharedModuleSetup(ModuleRules Module, ReadOnlyTargetRules Target)
    {
        // This is to emulate engine installation and verify includes during development
        if (Target.Configuration == UnrealTargetConfiguration.DebugGame
            || Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            Module.bUseUnity = false;
            Module.bTreatAsEngineModule = true;
        }
    }
}