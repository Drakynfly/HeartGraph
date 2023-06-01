// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkit.h"

#include "AssetTypeCategories.h"

class UHeartGraphNode;
class UHeartEdGraphNode;
class FHeartGraphAssetEditor;

DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphNode>, FOnGetSlateGraphWidgetInstance, UHeartEdGraphNode* Node);

DECLARE_LOG_CATEGORY_EXTERN(LogHeartEditor, Log, All)

class HEARTEDITOR_API FHeartEditorModule : public IModuleInterface
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

private:
    void RegisterPropertyCustomizations();
    void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance& DetailLayout);

    void OnAssetManagerCreated();
    void AddRegistrarPrimaryAssetRule();
    void DisableGraphNodeRegistrarError();

    /** Property Customizations; Cached so they can be unregistered */
    TSet<FName> PropertyCustomizations;

public:
    FDelegateHandle ModulesChangedHandle;

private:
    TSet<FName> CustomClassLayouts;

public:
    static EAssetTypeCategories::Type HeartAssetCategory;
private:
    TArray<TSharedRef<class IAssetTypeActions>> RegisteredAssetActions;

    TMap<FName, FOnGetSlateGraphWidgetInstance> EditorSlateCallbacks;

    TMap<TSubclassOf<UHeartGraphNode>, TSubclassOf<UHeartEdGraphNode>> HeartGraphNodeToEdGraphNodeClassMap;
};
