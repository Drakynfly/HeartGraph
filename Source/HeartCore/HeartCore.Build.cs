// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartCore : ModuleRules
{
    public HeartCore(ReadOnlyTargetRules Target) : base(Target)
    {
        ApplySharedModuleSetup(this, Target);

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "GameplayTags",
                "InputCore"
            });

        // Project dependencies
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Blood"
            });

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            });
    }

    public static void ApplySharedModuleSetup(ModuleRules Module, ReadOnlyTargetRules Target)
    {
        Module.PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        Module.DefaultBuildSettings = BuildSettingsVersion.Latest;
        Module.IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        // This is to emulate engine installation and verify includes during development
        if (Target.Configuration == UnrealTargetConfiguration.DebugGame
            || Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            Module.bUseUnity = false;
            Module.bTreatAsEngineModule = true;
            Module.bEnableNonInlinedGenCppWarnings = true;
            Module.UnsafeTypeCastWarningLevel = WarningLevel.Warning;
        }
    }
}