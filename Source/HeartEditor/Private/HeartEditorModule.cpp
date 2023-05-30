// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorModule.h"
#include "HeartEditorShared.h"
#include "HeartEditorStyle.h"

#include "Engine/AssetManager.h"
#include "Engine/AssetManagerSettings.h"
#include "GameplayTagsEditorModule.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/MessageLog.h"
#include "Modules/ModuleManager.h"

//#include "Graph/HeartGraphIndexer.h"

#include "HeartGraphSettings.h"

#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartRegistrationClasses.h"
#include "GraphRegistry/GraphNodeRegistrar.h"

#include "Graph/Widgets/Nodes/SHeartGraphNodeBase.h"
#include "Graph/Widgets/Nodes/SHeartGraphNode_Horizontal.h"
#include "Graph/Widgets/Nodes/SHeartGraphNode_Vertical.h"

#include "Nodes/AssetTypeActions_HeartGraphNodeBlueprint.h"

#include "Customizations/ItemsArrayCustomization.h"
#include "Nodes/HeartGraphNodeCustomization.h"
#include "Graph/HeartGraphSchemaCustomization.h"

#include "UI/HeartWidgetInputBindingAsset.h"

// @todo temp includes
#include "AssetToolsModule.h"
#include "Graph/AssetTypeActions_HeartGraphBlueprint.h"


static const FName PropertyEditorModuleName("PropertyEditor");
static const FName AssetSearchModuleName("AssetSearch");

DEFINE_LOG_CATEGORY(LogHeartEditor);

#define LOCTEXT_NAMESPACE "HeartEditorModule"

// @TODO TEMP
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
	//RegisterCustomClassLayout(UHeartGraph::StaticClass(),
	//	FOnGetDetailCustomizationInstance::CreateStatic(&FHeartGraphDetails::MakeInstance));
	RegisterCustomClassLayout(UHeartGraphNode::StaticClass(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FHeartGraphNodeCustomization::MakeInstance));
	RegisterCustomClassLayout(UHeartGraphSchema::StaticClass(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FHeartGraphSchemaCustomization::MakeInstance));

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	PropertyModule.NotifyCustomizationModuleChanged();

	// register asset indexers
	if (FModuleManager::Get().IsModuleLoaded(AssetSearchModuleName))
	{
		RegisterAssetIndexers();
	}

	RegisterSlateEditorWidget("Horizontal",
		FOnGetSlateGraphWidgetInstance::CreateStatic(&SHeartGraphNodeBase::MakeInstance<SHeartGraphNode_Horizontal>));

	RegisterSlateEditorWidget("Vertical",
		FOnGetSlateGraphWidgetInstance::CreateStatic(&SHeartGraphNodeBase::MakeInstance<SHeartGraphNode_Vertical>));

	ModulesChangedHandle = FModuleManager::Get().OnModulesChanged().AddRaw(this, &FHeartEditorModule::ModulesChangesCallback);

	// Register to get a warning on startup if settings aren't configured correctly
	UAssetManager::CallOrRegister_OnAssetManagerCreated(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FHeartEditorModule::OnAssetManagerCreated));
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

		for (const FName Key : PropertyCustomizations)
		{
			PropertyModule.UnregisterCustomPropertyTypeLayout(Key);
		}

		for (const FName Key : CustomClassLayouts)
		{
			PropertyModule.UnregisterCustomClassLayout(Key);
		}
	}

	FModuleManager::Get().OnModulesChanged().Remove(ModulesChangedHandle);
}

void FHeartEditorModule::RegisterSlateEditorWidget(const FName Style, const FOnGetSlateGraphWidgetInstance& Callback)
{
	EditorSlateCallbacks.Add(Style, Callback);
}

void FHeartEditorModule::DeregisterSlateEditorWidget(const FName Style)
{
	EditorSlateCallbacks.Remove(Style);
}

TArray<FName> FHeartEditorModule::GetSlateStyles() const
{
	TArray<FName> Out;
	EditorSlateCallbacks.GenerateKeyArray(Out);
	return Out;
}

TSharedPtr<SGraphNode> FHeartEditorModule::MakeVisualWidget(const FName Style, UHeartEdGraphNode* Node) const
{
	if (const FOnGetSlateGraphWidgetInstance* Callback = EditorSlateCallbacks.Find(Style))
	{
		return Callback->Execute(Node);
	}
	return nullptr;
}

void FHeartEditorModule::RegisterEdGraphNode(const TSubclassOf<UHeartGraphNode> HeartClass,
											 const TSubclassOf<UHeartEdGraphNode> EdClass)
{
	HeartGraphNodeToEdGraphNodeClassMap.Add(HeartClass, EdClass);
}

void FHeartEditorModule::DeregisterEdGraphNode(const TSubclassOf<UHeartGraphNode> HeartClass)
{
	HeartGraphNodeToEdGraphNodeClassMap.Remove(HeartClass);
}

TSubclassOf<UHeartEdGraphNode> FHeartEditorModule::GetEdGraphClass(const TSubclassOf<UHeartGraphNode> HeartClass) const
{
	if (!HeartGraphNodeToEdGraphNodeClassMap.IsEmpty())
	{
		for (UClass* Test = HeartClass;
			Test && Test != UObject::StaticClass();
			Test = Test->GetSuperClass())
		{
			if (auto&& FoundClass = HeartGraphNodeToEdGraphNodeClassMap.Find(Test))
			{
				return *FoundClass;
			}
		}
	}

	return UHeartEdGraphNode::StaticClass();
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

	Customizations.Add(FHeartGraphPinTag::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGameplayTagCustomizationPublic::MakeInstance));

	Customizations.Add(FHeartGraphNodeEditorDataTemp::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartGraphNodeEditorDataTempCustomization::MakeInstance));

	// Register property customizations
	for (auto&& Customization : Customizations)
	{
		PropertyCustomizations.Add(Customization.Key);
		PropertyModule.RegisterCustomPropertyTypeLayout(Customization.Key, Customization.Value);
	}

	// notify on customization change
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FHeartEditorModule::RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance& DetailLayout)
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

void FHeartEditorModule::OnAssetManagerCreated()
{
	// Make sure the project has a asset manager configuration for Registrars or we won't be able to load them at runtime
	const FPrimaryAssetId DummyGraphNodeRegistrarAssetId(UGraphNodeRegistrar::StaticClass()->GetFName(), NAME_None);
	const FPrimaryAssetRules GameDataRules = UAssetManager::Get().GetPrimaryAssetRules(DummyGraphNodeRegistrarAssetId);
	if (GameDataRules.IsDefault() && !GetDefault<UHeartGraphSettings>()->DisableAssetRegistryError)
	{
		FMessageLog("LoadErrors").Error()
			->AddToken(FTextToken::Create(FText::Format(
				LOCTEXT("MissingRuleForGraphNodeRegistrar", "Asset Manager does not have a rule for assets of type {0}. They will not be discoverable at runtime!"),
				FText::FromName(UGraphNodeRegistrar::StaticClass()->GetFName()))))
			->AddToken(FActionToken::Create(
				LOCTEXT("AddRuleForGraphNodeRegistrar", "Add entry to PrimaryAssetTypesToScan?"), FText(),
				FOnActionTokenExecuted::CreateRaw(this, &FHeartEditorModule::AddRegistrarPrimaryAssetRule), true))
			->AddToken(FActionToken::Create(
				LOCTEXT("DisableGraphNodeRegistrarError", "Disable this error, "), FText(),
				FOnActionTokenExecuted::CreateRaw(this, &FHeartEditorModule::DisableGraphNodeRegistrarError), true));
	}
}

void FHeartEditorModule::AddRegistrarPrimaryAssetRule()
{
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();

	const FString& ConfigFileName = Settings->GetDefaultConfigFilename();

	// Check out the ini or make it writable
	if (Heart::EditorShared::CheckOutFile(ConfigFileName, true))
	{
		// Add the rule to project settings
		FPrimaryAssetTypeInfo NewTypeInfo(
			UGraphNodeRegistrar::StaticClass()->GetFName(),
			UGraphNodeRegistrar::StaticClass(),
			false,
			false);
		NewTypeInfo.Rules.CookRule = EPrimaryAssetCookRule::AlwaysCook;

		NewTypeInfo.GetDirectories().Add(FDirectoryPath{"/Game"});

		// If this project has the DemoContent plugin installed, automatically include that root too.
		if (IPluginManager::Get().FindPlugin("HeartDemoContent"))
		{
			NewTypeInfo.GetDirectories().Add(FDirectoryPath{"/HeartDemoContent"});
		}

		Settings->Modify(true);

		Settings->PrimaryAssetTypesToScan.Add(NewTypeInfo);

 		Settings->PostEditChange();
		Settings->TryUpdateDefaultConfigFile();

		UAssetManager::Get().ReinitializeFromConfig();
	}
}

void FHeartEditorModule::DisableGraphNodeRegistrarError()
{
	UHeartGraphSettings* Settings = GetMutableDefault<UHeartGraphSettings>();

	const FString& ConfigFileName = Settings->GetDefaultConfigFilename();

	// Check out the ini or make it writable
	if (Heart::EditorShared::CheckOutFile(ConfigFileName, true))
	{
		Settings->Modify(true);

		Settings->DisableAssetRegistryError = true;

		Settings->PostEditChange();
		Settings->TryUpdateDefaultConfigFile();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartEditorModule, HeartEditor)