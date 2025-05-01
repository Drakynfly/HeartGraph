// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class Blood : ModuleRules
{
    public Blood(ReadOnlyTargetRules Target) : base(Target)
    {
        ApplySharedModuleSetup(this, Target);

        // IDK, something about this module is broken, and if Unity is enabled it blows up :?
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core"
            });

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine"
            });

        PublicDefinitions.Add("ALLOCATE_BLOOD_STATICS=0");
    }

    public static void ApplySharedModuleSetup(ModuleRules Module, ReadOnlyTargetRules Target)
    {
        Module.PCHUsage = PCHUsageMode.NoPCHs;
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