using UnrealBuildTool;

public class HeartCanvas : ModuleRules
{
    public HeartCanvas(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputCore",
                "UMG"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "HeartCore",
                "Heart",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}