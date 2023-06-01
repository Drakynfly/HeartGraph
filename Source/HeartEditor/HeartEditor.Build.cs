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
                "EditorSubsystem",
                "GameplayTags",
                "GameplayTagsEditor",   // For FGameplayTagCustomizationPublic
                "HeartCore",
                "HeartCoreEditor",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "ApplicationCore",  // For clipboard copy/paste
                "AssetSearch",
                "AssetTools",
                "ClassViewer",
                "Core",
                "CoreUObject",
                "EditorFramework",  // For FToolkitManager
                "EditorStyle",
                "Engine",
                "GraphEditor",      // For SGraphPalette
                "InputCore",
                "KismetWidgets",    // For SLevelOfDetailBranchNode
                "Projects",         // For IPluginManager
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "SourceControl",    // For Diffing utils
                "ToolMenus",
                "UnrealEd"
            });
    }
}