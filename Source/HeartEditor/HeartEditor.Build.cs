// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartEditor : ModuleRules
{
    public HeartEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "HeartCore",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "ApplicationCore", // For clipboard copy/paste
            "AssetSearch",
            "AssetTools",
            //"BlueprintGraph",
            "ClassViewer",
            //"ContentBrowser",
            "Core",
            "CoreUObject",
            //"DetailCustomizations",
            //"DeveloperSettings",
            "EditorFramework", // For FToolkitManager
            //"EditorStyle",
            "Engine",
            "GraphEditor", // For SGraphPalette
            "InputCore",
            //"Json",
            //"JsonUtilities",
            "Kismet",
            "KismetWidgets", // For SLevelOfDetailBranchNode
            "Projects", // For IPluginManager
            "PropertyEditor",
            //"RenderCore",
            "Slate",
            "SlateCore",
            "SourceControl", // For Diffing utils
            "ToolMenus",
            "UnrealEd"
        });
    }
}