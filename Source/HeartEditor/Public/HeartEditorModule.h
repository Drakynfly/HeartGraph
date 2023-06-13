// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkit.h"

// @todo this include is temp
#include "AssetTypeCategories.h"

class UHeartGraph;
class UHeartGraphNode;
class UHeartEdGraphNode;
class FApplicationMode;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartEditor, Log, All)

namespace Heart::AssetEditor
{
    class FHeartGraphEditor;
}

DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphNode>, FOnGetSlateGraphWidgetInstance, UHeartEdGraphNode* Node);

struct HEARTEDITOR_API FHeartRegisteredApplicationMode
{
	FText LocalizedName;
	FText TooltipText;

    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FApplicationMode>, FOnGetInstance, TSharedRef<Heart::AssetEditor::FHeartGraphEditor> Editor);
    FOnGetInstance CreateModeInstance;

    DECLARE_DELEGATE_RetVal_OneParam(bool, FSupportsAssetCallback, const UHeartGraph* Asset);
    FSupportsAssetCallback SupportsAsset;
};

class HEARTEDITOR_API FHeartEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    void RegisterSlateEditorWidget(FName Style, const FOnGetSlateGraphWidgetInstance& Callback);
    void DeregisterSlateEditorWidget(FName Style);
    TArray<FName> GetSlateStyles() const;
    TSharedPtr<SGraphNode> MakeVisualWidget(FName Style, UHeartEdGraphNode* Node) const;

    void RegisterEdGraphNode(TSubclassOf<UHeartGraphNode> HeartClass, TSubclassOf<UHeartEdGraphNode> EdClass);
    void DeregisterEdGraphNode(TSubclassOf<UHeartGraphNode> HeartClass);
    TSubclassOf<UHeartEdGraphNode> GetEdGraphClass(TSubclassOf<UHeartGraphNode> HeartClass) const;

    /** Gets the extensibility managers for outside entities to extend the Heart Asset Editor's menus and toolbars */
    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
    virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

    void RegisterApplicationMode(FName ModeName, const FHeartRegisteredApplicationMode& ModeData);
    void DeregisterApplicationMode(FName ModeName);
    const TMap<FName, FHeartRegisteredApplicationMode>& GetApplicationModes() const { return ApplicationModeCallbacks; }

private:
    void RegisterPropertyCustomizations();
    void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance& DetailLayout);

    void ModulesChangesCallback(FName ModuleName, EModuleChangeReason ReasonForChange);
    void RegisterAssetIndexers() const;

    void OnAssetManagerCreated();
    void AddRegistrarPrimaryAssetRule();
    void DisableGraphNodeRegistrarError();

    /** Property Customizations; Cached so they can be unregistered */
    TSet<FName> PropertyCustomizations;

public:
    FDelegateHandle ModulesChangedHandle;

private:
    TSet<FName> CustomClassLayouts;

    TMap<TSubclassOf<UHeartGraphNode>, TSubclassOf<UHeartEdGraphNode>> HeartGraphNodeToEdGraphNodeClassMap;

    // This holds the registered callbacks to generate Slate widgets for UHeartEdGraphNodes
    TMap<FName, FOnGetSlateGraphWidgetInstance> EditorSlateCallbacks;

    // This holds the registered callbacks to generate Application Modes for UHeartEdGraphNodes
    TMap<FName, FHeartRegisteredApplicationMode> ApplicationModeCallbacks;

    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
    TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

    // @todo TEMP STUFF:
public:
    static EAssetTypeCategories::Type HeartAssetCategory_TEMP;
private:
    TArray<TSharedRef<class IAssetTypeActions>> RegisteredAssetActions;
};
