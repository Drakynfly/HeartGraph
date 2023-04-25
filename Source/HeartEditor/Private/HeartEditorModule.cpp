// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorModule.h"
#include "HeartEditorStyle.h"

#include "Graph/HeartGraphAssetEditor.h"
//#include "Asset/HeartAssetIndexer.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "UI/HeartWidgetInputBindingAsset.h"

#include "IAssetSearchModule.h"
#include "GraphRegistry/HeartRegistrationClasses.h"
#include "Modules/ModuleManager.h"

#include "Customizations/ItemsArrayCustomization.h"

#include "GameplayTagsEditorModule.h"

// @todo temp includes
#include "AssetToolsModule.h"
#include "Graph/AssetTypeActions_HeartGraphBlueprint.h"
#include "Nodes/AssetTypeActions_HeartGraphNodeBlueprint.h"


static const FName PropertyEditorModuleName("PropertyEditor");
static const FName AssetSearchModuleName("AssetSearch");

DEFINE_LOG_CATEGORY(LogHeartEditor);

#define LOCTEXT_NAMESPACE "HeartEditorModule"

EAssetTypeCategories::Type FHeartEditorModule::HeartAssetCategory_TEMP = static_cast<EAssetTypeCategories::Type>(0);

void FHeartEditorModule::StartupModule()
{
	FHeartEditorStyle::Initialize();

	// @TODO BEGIN TEMP STUFF
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		FText AssetCategoryText = LOCTEXT("HeartAssetCategory", "Heart");

		if (!AssetCategoryText.IsEmpty())
		{
			if (HeartAssetCategory_TEMP == EAssetTypeCategories::None)
			{
				HeartAssetCategory_TEMP = AssetTools.RegisterAdvancedAssetCategory(FName("Heart"), AssetCategoryText);
			}
		}

		RegisteredAssetActions.Add(MakeShareable(new FAssetTypeActions_HeartGraphBlueprint()));
		RegisteredAssetActions.Add(MakeShareable(new FAssetTypeActions_HeartGraphNodeBlueprint()));

		for (auto&& TypeActions : RegisteredAssetActions)
		{
			AssetTools.RegisterAssetTypeActions(TypeActions);
		}
	}
	// @TODO END TEMP STUFF

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

	// @TODO BEGIN TEMP STUFF
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			for (auto&& TypeActions : RegisteredAssetActions)
			{
				AssetTools.UnregisterAssetTypeActions(TypeActions);
			}
		}

		RegisteredAssetActions.Empty();
	}
	// @TODO END TEMP STUFF

	// Unregister customizations
	if (FModuleManager::Get().IsModuleLoaded(PropertyEditorModuleName))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);

		for (FName Key : PropertyCustomizations)
		{
			PropertyModule.UnregisterCustomPropertyTypeLayout(Key);
		}

		for (FName Key : CustomClassLayouts)
		{
			PropertyModule.UnregisterCustomClassLayout(Key);
		}
	}

	FModuleManager::Get().OnModulesChanged().Remove(ModulesChangedHandle);
}

TSharedRef<FHeartGraphAssetEditor> FHeartEditorModule::CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph)
{
	TSharedRef<FHeartGraphAssetEditor> NewHeartGraphAssetEditor(new FHeartGraphAssetEditor());
	NewHeartGraphAssetEditor->InitHeartGraphAssetEditor(Mode, InitToolkitHost, HeartGraph);
	return NewHeartGraphAssetEditor;
}

void FHeartEditorModule::RegisterPropertyCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);

	TMap<FName, FOnGetPropertyTypeCustomizationInstance> Customizations;

	Customizations.Add(FClassList::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Heart::FItemsArrayCustomization::MakeInstance));

	Customizations.Add(FHeartWidgetInputBinding::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Heart::FItemsArrayCustomization::MakeInstance,
			Heart::FItemsArrayCustomization::FArgs{GET_MEMBER_NAME_CHECKED(FHeartWidgetInputBinding, InputHandler),
												   GET_MEMBER_NAME_CHECKED(FHeartWidgetInputBinding, Triggers)}));

	//Customizations.Add(FHeartGraphPinTag::StaticStruct()->GetFName(),
	//	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGameplayTagCustomizationPublic::MakeInstance));

	// Register property customizations
	for (auto&& Customization : Customizations)
	{
		PropertyCustomizations.Add(Customization.Key);
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartEditorModule, HeartEditor)