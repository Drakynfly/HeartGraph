// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

// @todo this include is temp
#include "AssetTypeCategories.h"

#include "Modules/ModuleInterface.h"
#include "PropertyEditorDelegates.h"
#include "Toolkits/IToolkit.h"

class FHeartGraphAssetEditor;
class UHeartGraph;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartEditor, Log, All)

class HEARTEDITOR_API FHeartEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    static TSharedRef<FHeartGraphAssetEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph);

private:
    void RegisterPropertyCustomizations();
    void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance DetailLayout);

    void ModulesChangesCallback(FName ModuleName, EModuleChangeReason ReasonForChange);
    void RegisterAssetIndexers() const;

    /** Property Customizations; Cached so they can be unregistered */
    TMap<FName, FOnGetPropertyTypeCustomizationInstance> PropertyCustomizations;

public:
    FDelegateHandle ModulesChangedHandle;

private:
    TSet<FName> CustomClassLayouts;

    // @todo TEMP STUFF:
public:
    static EAssetTypeCategories::Type HeartAssetCategory_TEMP;
private:
    TArray<TSharedRef<class IAssetTypeActions>> RegisteredAssetActions;
};
