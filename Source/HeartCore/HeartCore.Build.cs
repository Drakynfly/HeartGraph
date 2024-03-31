// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartCore : ModuleRules
{
    public HeartCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        ApplySharedModuleSetup(this, Target);

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "GameplayTags",
                "InputCore",
                "StructUtils"
            });

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
            Module.bEnableNonInlinedGenCppWarnings = true;
        }
    }
}