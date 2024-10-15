// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;

public class HeartEditor : ModuleRules
{
    public HeartEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        HeartCore.ApplySharedModuleSetup(this, Target);

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "EditorSubsystem",
                "GameplayTags",
                "GameplayTagsEditor",   // For FGameplayTagCustomizationPublic
                "Blood",
                "HeartCore",
                "HeartCoreEditor",
                "Heart",
                "HeartCanvas" // @todo i exceedingly don't like this. Either we should move stuff out of HeartCanvas that we depend on, *or*, move our dependant logic to HeartCanvasEditor (but that's unlikely)
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
                "EngineAssetDefinitions",
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