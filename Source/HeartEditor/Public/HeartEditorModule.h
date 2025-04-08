// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkit.h"

class FApplicationMode;
class UHeartEdGraphNode;
class UHeartEdGraphSchema;
class UHeartGraph;
class UHeartGraphNode;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartEditor, Log, All)

struct FConnectionDrawingPolicyCtorPack
{
    int32 InBackLayerID;
    int32 InFrontLayerID;
    float InZoomFactor;
    const FSlateRect& InClippingRect;
    FSlateWindowElementList& InDrawElements;
    UEdGraph* InGraphObj;
};

namespace Heart::AssetEditor
{
    class FHeartGraphEditor;

    struct HEARTEDITOR_API FRegisteredApplicationMode
    {
        FText LocalizedName;
        FText TooltipText;

        using FOnGetInstance = TDelegate<TSharedRef<FApplicationMode>(TSharedRef<FHeartGraphEditor> Editor)>;
        FOnGetInstance CreateModeInstance;

        using FSupportsAssetCallback = TDelegate<bool(const UHeartGraph* Asset)>;
        FSupportsAssetCallback SupportsAsset;
    };

    template <
        typename T
        UE_REQUIRES(TIsDerivedFrom<T, FConnectionDrawingPolicy>::Value)
    >
    FConnectionDrawingPolicy* MakeDrawingPolicyInstance(const UHeartEdGraphSchema* Schema, const FConnectionDrawingPolicyCtorPack& CtorPack)
    {
        return new T(CtorPack.InBackLayerID, CtorPack.InFrontLayerID, CtorPack.InZoomFactor,
            CtorPack.InClippingRect, CtorPack.InDrawElements, CtorPack.InGraphObj);
    }
}

using FOnGetSlateGraphWidgetInstance = TDelegate<TSharedRef<SGraphNode>(UHeartEdGraphNode* Node)>;
using FOnGetDrawingPolicyInstance = TDelegate<FConnectionDrawingPolicy*(const UHeartEdGraphSchema* Schema, const FConnectionDrawingPolicyCtorPack&)>;

class HEARTEDITOR_API FHeartEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    // @todo this is great stuff to move to the EDITOR REGISTRY tho

    void RegisterSlateEditorWidget(FName Style, const FOnGetSlateGraphWidgetInstance& Callback);
    void DeregisterSlateEditorWidget(FName Style);
    TArray<FName> GetSlateStyles() const;
    TSharedPtr<SGraphNode> MakeSlateWidget(FName Style, UHeartEdGraphNode* Node) const;

    void RegisterConnectionDrawingPolicy(FName Style, const FOnGetDrawingPolicyInstance& Callback);
    void DeregisterConnectionDrawingPolicy(FName Style);
    TArray<FName> GetDrawingPolicies() const;
    FConnectionDrawingPolicy* GetDrawingPolicyInstance(FName Style, const UHeartEdGraphSchema* Schema, const FConnectionDrawingPolicyCtorPack& CtorPack) const;

    void RegisterEdGraphNode(const TSubclassOf<UHeartGraphNode>& HeartClass, const TSubclassOf<UHeartEdGraphNode>& EdClass);
    void DeregisterEdGraphNode(const TSubclassOf<UHeartGraphNode>& HeartClass);
    TSubclassOf<UHeartEdGraphNode> GetEdGraphClass(const TSubclassOf<UHeartGraphNode>& HeartClass) const;

    /** Gets the extensibility managers for outside entities to extend the Heart Asset Editor's menus and toolbars */
    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
    virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

    void RegisterApplicationMode(FName ModeName, const Heart::AssetEditor::FRegisteredApplicationMode& ModeData);
    void DeregisterApplicationMode(FName ModeName);
    const TMap<FName, Heart::AssetEditor::FRegisteredApplicationMode>& GetApplicationModes() const { return ApplicationModeCallbacks; }

private:
    void RegisterPropertyCustomizations();
    void RegisterCustomClassLayout(const TSubclassOf<UObject>& Class, const FOnGetDetailCustomizationInstance& DetailLayout);

    void ModulesChangesCallback(FName ModuleName, EModuleChangeReason ReasonForChange);
    void RegisterAssetIndexers() const;

    void OnAssetManagerCreated();
    void AddRegistrarPrimaryAssetRule();
    void DisableGraphNodeRegistrarError();

    /** Property Customizations; Cached, so they can be unregistered */
    TSet<FName> PropertyCustomizations;

public:
    FDelegateHandle ModulesChangedHandle;

private:
    TSet<FName> CustomClassLayouts;

    TMap<TSubclassOf<UHeartGraphNode>, TSubclassOf<UHeartEdGraphNode>> HeartGraphNodeToEdGraphNodeClassMap;

    // This holds the registered callbacks to generate Slate widgets for UHeartEdGraphNodes
    TMap<FName, FOnGetSlateGraphWidgetInstance> EditorSlateCallbacks;

    // This holds the registered callbacks to create drawing policies for UHeartEdGraphSchema
    TMap<FName, FOnGetDrawingPolicyInstance> DrawingPolicyCallbacks;

    // This holds the registered callbacks to generate Application Modes for UHeartEdGraphNodes
    TMap<FName, Heart::AssetEditor::FRegisteredApplicationMode> ApplicationModeCallbacks;

    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
    TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
};