// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorModule.h"
#include "HeartEditorStyle.h"

#include "Graph/AssetTypeActions_HeartGraph.h"
#include "Graph/HeartGraphAssetEditor.h"
//#include "Asset/HeartAssetIndexer.h"
#include "Graph/AssetTypeActions_HeartGraphBlueprint.h"
#include "Nodes/AssetTypeActions_HeartGraphNodeBlueprint.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"
#include "IAssetSearchModule.h"
#include "Customizations/HeartWidgetInputBindingCustomization.h"
#include "GraphRegistry/HeartRegistrationClasses.h"
#include "Modules/ModuleManager.h"

#include "Customizations/ItemsArrayCustomization.h"
#include "UI/HeartWidgetInputBindingAsset.h"

static FName PropertyEditorModuleName = TEXT("PropertyEditor");
static FName AssetToolsModuleName = TEXT("AssetTools");
static FName AssetSearchModuleName = TEXT("AssetSearch");

DEFINE_LOG_CATEGORY(LogHeartEditor);

#define LOCTEXT_NAMESPACE "HeartEditorModule"

EAssetTypeCategories::Type FHeartEditorModule::HeartAssetCategory_TEMP = static_cast<EAssetTypeCategories::Type>(0);

void FHeartEditorModule::StartupModule()
{
	FHeartEditorStyle::Initialize();

	RegisterAssets();

	RegisterPropertyCustomizations();

	// register detail customizations
	//RegisterCustomClassLayout(UHeartGraph::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FHeartGraphDetails::MakeInstance));
	//RegisterCustomClassLayout(UHeartGraphNode::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FHeartGraphNodeDetails::MakeInstance));

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	PropertyModule.NotifyCustomizationModuleChanged();

	// register asset indexers
	if (FModuleManager::Get().IsModuleLoaded(AssetSearchModuleName))
	{
		RegisterAssetIndexers();
	}
	ModulesChangedHandle = FModuleManager::Get().OnModulesChanged().AddRaw(this, &FHeartEditorModule::ModulesChangesCallback);
}

void FHeartEditorModule::ShutdownModule()
{
	FHeartEditorStyle::Shutdown();

	UnregisterAssets();

	// unregister details customizations
	if (FModuleManager::Get().IsModuleLoaded(PropertyEditorModuleName))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);

		for (auto It = CustomClassLayouts.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomClassLayout(*It);
			}
		}
	}

	FModuleManager::Get().OnModulesChanged().Remove(ModulesChangedHandle);
}

void FHeartEditorModule::RegisterAssets()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();

	FText AssetCategoryText = LOCTEXT("HeartAssetCategory", "Heart");

	if (!AssetCategoryText.IsEmpty())
	{
		if (HeartAssetCategory_TEMP == EAssetTypeCategories::None)
		{
			HeartAssetCategory_TEMP = AssetTools.RegisterAdvancedAssetCategory(FName("Heart"), AssetCategoryText);
		}
	}

	RegisteredAssetActions.Add(MakeShareable(new FAssetTypeActions_HeartGraph()));
	RegisteredAssetActions.Add(MakeShareable(new FAssetTypeActions_HeartGraphBlueprint()));
	RegisteredAssetActions.Add(MakeShareable(new FAssetTypeActions_HeartGraphNodeBlueprint()));

	for (auto&& TypeActions : RegisteredAssetActions)
	{
		AssetTools.RegisterAssetTypeActions(TypeActions);
	}
}

void FHeartEditorModule::UnregisterAssets()
{
	if (FModuleManager::Get().IsModuleLoaded(AssetToolsModuleName))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
		for (auto&& TypeActions : RegisteredAssetActions)
		{
			AssetTools.UnregisterAssetTypeActions(TypeActions);
		}
	}

	RegisteredAssetActions.Empty();
}

void FHeartEditorModule::RegisterPropertyCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);

	PropertyCustomizations.Add(FClassList::StaticStruct()->GetFName(),
	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FItemsArrayCustomization::MakeInstance));

	PropertyCustomizations.Add(FHeartWidgetInputBinding::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartWidgetInputBindingCustomization::MakeInstance));

	// Register property customizations
	for (auto&& Customization : PropertyCustomizations)
	{
		PropertyModule.RegisterCustomPropertyTypeLayout(Customization.Key, Customization.Value);
	}

	// notify on customization change
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FHeartEditorModule::RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance DetailLayout)
{
	if (Class)
	{
		CustomClassLayouts.Add(Class->GetFName());

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
		PropertyModule.RegisterCustomClassLayout(Class->GetFName(), DetailLayout);
	}
}

void FHeartEditorModule::ModulesChangesCallback(FName ModuleName, EModuleChangeReason ReasonForChange)
{
	if (ReasonForChange == EModuleChangeReason::ModuleLoaded && ModuleName == AssetSearchModuleName)
	{
		RegisterAssetIndexers();
	}
}

void FHeartEditorModule::RegisterAssetIndexers() const
{
	// @todo
	//IAssetSearchModule::Get().RegisterAssetIndexer(UHeartGraph::StaticClass(), MakeUnique<FHeartAssetIndexer>());
}

TSharedRef<FHeartGraphAssetEditor> FHeartEditorModule::CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph)
{
	TSharedRef<FHeartGraphAssetEditor> NewHeartGraphAssetEditor(new FHeartGraphAssetEditor());
	NewHeartGraphAssetEditor->InitHeartGraphAssetEditor(Mode, InitToolkitHost, HeartGraph);
	return NewHeartGraphAssetEditor;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartEditorModule, HeartEditor)