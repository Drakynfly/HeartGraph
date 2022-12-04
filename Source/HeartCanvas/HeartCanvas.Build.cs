using UnrealBuildTool;

public class HeartCanvas : ModuleRules
{
    public HeartCanvas(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "InputCore",
                "UMG"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "HeartCore",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "StructUtils"
            }
        );
    }
}