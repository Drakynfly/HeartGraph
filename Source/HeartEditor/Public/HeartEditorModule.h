// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

// @todo this include is temp
#include "AssetTypeCategories.h"

#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkit.h"

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

private:
    void RegisterPropertyCustomizations();
    void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance& DetailLayout);

    void ModulesChangesCallback(FName ModuleName, EModuleChangeReason ReasonForChange);
    void RegisterAssetIndexers() const;

    /** Property Customizations; Cached so they can be unregistered */
    TSet<FName> PropertyCustomizations;

public:
    FDelegateHandle ModulesChangedHandle;

private:
    TSet<FName> CustomClassLayouts;

    // @todo TEMP STUFF:
public:
    static EAssetTypeCategories::Type HeartAssetCategory_TEMP;
private:
    TArray<TSharedRef<class IAssetTypeActions>> RegisteredAssetActions;

    TMap<FName, FOnGetSlateGraphWidgetInstance> EditorSlateCallbacks;
};
