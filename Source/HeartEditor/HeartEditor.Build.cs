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
                "HeartCore",
                "HeartCoreEditor",
                "Heart"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "ApplicationCore",  // For clipboard copy/paste
                "AssetDefinition",
                "AssetSearch",
                "AssetTools",
                "ClassViewer",
                "Core",
                "CoreUObject",
                "EditorFramework",  // For FToolkitManager
                "Engine",
                "GameplayTagsEditor", // For FGameplayTagCustomizationPublic
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