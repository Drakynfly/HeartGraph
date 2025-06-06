﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

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

#include "GraphRegistry/GraphNodeRegistrar.h"

#include "Graph/Widgets/Nodes/SHeartGraphNodeBase.h"
#include "Graph/Widgets/Nodes/SHeartGraphNode_Horizontal.h"
#include "Graph/Widgets/Nodes/SHeartGraphNode_Vertical.h"

#include "Customizations/ItemsArrayCustomization.h"
#include "Nodes/HeartGraphNodeCustomization.h"
#include "Graph/HeartGraphSchemaCustomization.h"

#include "Customizations/HeartGuidCustomization.h"

#include "AssetEditor/ApplicationMode_Editor.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/Widget/ConnectionDrawingPolicies/DebugConnectionDrawingPolicy.h"
#include "Input/HeartInputBindingAsset.h"

static const FLazyName PropertyEditorModuleName("PropertyEditor");
static const FLazyName AssetSearchModuleName("AssetSearch");

DEFINE_LOG_CATEGORY(LogHeartEditor);

#define LOCTEXT_NAMESPACE "HeartEditorModule"

FConnectionDrawingPolicy* MakeDefaultDrawingPolicy(const UHeartEdGraphSchema* Schema, const FConnectionDrawingPolicyCtorPack& CtorPack)
{
	return Schema->UEdGraphSchema::CreateConnectionDrawingPolicy(
		CtorPack.InBackLayerID, CtorPack.InFrontLayerID, CtorPack.InZoomFactor,
		CtorPack.InClippingRect, CtorPack.InDrawElements, CtorPack.InGraphObj);
}

void FHeartEditorModule::StartupModule()
{
	using namespace Heart::AssetEditor;

	FHeartEditorStyle::Initialize();

	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();

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

	RegisterSlateEditorWidget(TEXT("Horizontal"),
		FOnGetSlateGraphWidgetInstance::CreateStatic(&SHeartGraphNodeBase::MakeInstance<SHeartGraphNode_Horizontal>));

	RegisterSlateEditorWidget(TEXT("Vertical"),
		FOnGetSlateGraphWidgetInstance::CreateStatic(&SHeartGraphNodeBase::MakeInstance<SHeartGraphNode_Vertical>));

	RegisterConnectionDrawingPolicy(TEXT("Default"),
	 FOnGetDrawingPolicyInstance::CreateStatic(&MakeDefaultDrawingPolicy));

	RegisterConnectionDrawingPolicy(TEXT("Debug"),
		FOnGetDrawingPolicyInstance::CreateStatic(&Heart::AssetEditor::MakeDrawingPolicyInstance<Heart::Editor::FDebugConnectionDrawingPolicy>));

	ModulesChangedHandle = FModuleManager::Get().OnModulesChanged().AddRaw(this, &FHeartEditorModule::ModulesChangesCallback);

	// Register to get a warning on startup if settings aren't configured correctly
	UAssetManager::CallOrRegister_OnAssetManagerCreated(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FHeartEditorModule::OnAssetManagerCreated));

	// Register the default application mode.
	FRegisteredApplicationMode EditorMode;
	EditorMode.LocalizedName = LOCTEXT("ApplicationMode_Editor.LocalizedName", "Graph");
	EditorMode.TooltipText = LOCTEXT("ApplicationMode_Editor.TooltipText", "Switch to Graph Editing Mode");
	EditorMode.CreateModeInstance.BindLambda(
		[](const TSharedRef<FHeartGraphEditor>& Editor)
		{
			return MakeShared<FApplicationMode_Editor>(Editor);
		});
	EditorMode.SupportsAsset.BindLambda(
		[](const UHeartGraph* Asset)
		{
			return true;
		});

	RegisterApplicationMode(FApplicationMode_Editor::ModeID, EditorMode);
}

void FHeartEditorModule::ShutdownModule()
{
	FHeartEditorStyle::Shutdown();

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

TSharedPtr<SGraphNode> FHeartEditorModule::MakeSlateWidget(const FName Style, UHeartEdGraphNode* Node) const
{
	if (const FOnGetSlateGraphWidgetInstance* Callback = EditorSlateCallbacks.Find(Style))
	{
		return Callback->Execute(Node);
	}
	return nullptr;
}

void FHeartEditorModule::RegisterConnectionDrawingPolicy(const FName Style, const FOnGetDrawingPolicyInstance& Callback)
{
	DrawingPolicyCallbacks.Add(Style, Callback);
}

void FHeartEditorModule::DeregisterConnectionDrawingPolicy(const FName Style)
{
	DrawingPolicyCallbacks.Remove(Style);
}

TArray<FName> FHeartEditorModule::GetDrawingPolicies() const
{
	TArray<FName> Out;
	DrawingPolicyCallbacks.GenerateKeyArray(Out);
	return Out;
}

FConnectionDrawingPolicy* FHeartEditorModule::GetDrawingPolicyInstance(const FName Style, const UHeartEdGraphSchema* Schema, const FConnectionDrawingPolicyCtorPack& CtorPack) const
{
	if (Style.IsNone())
	{
		return nullptr;
	}

	if (const FOnGetDrawingPolicyInstance* Callback = DrawingPolicyCallbacks.Find(Style))
	{
		return Callback->Execute(Schema, CtorPack);
	}

	UE_LOG(LogTemp, Warning, TEXT("Requested DrawingPolicy '%s' is not registered to module!"), *Style.ToString())
	return nullptr;
}

void FHeartEditorModule::RegisterEdGraphNode(const TSubclassOf<UHeartGraphNode>& HeartClass,
											 const TSubclassOf<UHeartEdGraphNode>& EdClass)
{
	HeartGraphNodeToEdGraphNodeClassMap.Add(HeartClass, EdClass);
}

void FHeartEditorModule::DeregisterEdGraphNode(const TSubclassOf<UHeartGraphNode>& HeartClass)
{
	HeartGraphNodeToEdGraphNodeClassMap.Remove(HeartClass);
}

TSubclassOf<UHeartEdGraphNode> FHeartEditorModule::GetEdGraphClass(const TSubclassOf<UHeartGraphNode>& HeartClass) const
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

void FHeartEditorModule::RegisterApplicationMode(const FName ModeName, const Heart::AssetEditor::FRegisteredApplicationMode& ModeData)
{
	ApplicationModeCallbacks.Add(ModeName, ModeData);
}

void FHeartEditorModule::DeregisterApplicationMode(const FName ModeName)
{
	ApplicationModeCallbacks.Remove(ModeName);
}

void FHeartEditorModule::RegisterPropertyCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);

	TMap<FName, FOnGetPropertyTypeCustomizationInstance> Customizations;

	Customizations.Add(FHeartGraphGuid::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartGuidCustomization::MakeInstance));
	Customizations.Add(FHeartExtensionGuid::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartGuidCustomization::MakeInstance));
	Customizations.Add(FHeartNodeGuid::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartGuidCustomization::MakeInstance));
	Customizations.Add(FHeartPinGuid::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHeartGuidCustomization::MakeInstance));

	//Customizations.Add(FClassList::StaticStruct()->GetFName(),
	//	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Heart::FItemsArrayCustomization::MakeInstance));

	Customizations.Add(FHeartBoundInput::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Heart::FItemsArrayCustomization::MakeInstance,
			Heart::FItemsArrayCustomization::FArgs{GET_MEMBER_NAME_CHECKED(FHeartBoundInput, InputHandler),
												   GET_MEMBER_NAME_CHECKED(FHeartBoundInput, Triggers)}));

	Customizations.Add(FHeartGraphPinTag::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGameplayTagCustomizationPublic::MakeInstance));

	// Register property customizations
	for (auto&& Customization : Customizations)
	{
		PropertyCustomizations.Add(Customization.Key);
		PropertyModule.RegisterCustomPropertyTypeLayout(Customization.Key, Customization.Value);
	}

	// notify on customization change
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FHeartEditorModule::RegisterCustomClassLayout(const TSubclassOf<UObject>& Class, const FOnGetDetailCustomizationInstance& DetailLayout)
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
	// Make sure the project has an asset manager configuration for Registrars, or we won't be able to load them at runtime
	const FPrimaryAssetId DummyGraphNodeRegistrarAssetId(UGraphNodeRegistrar::StaticClass()->GetFName(), NAME_None);
	const FPrimaryAssetRules GameDataRules = UAssetManager::Get().GetPrimaryAssetRules(DummyGraphNodeRegistrarAssetId);
	if (GameDataRules.IsDefault() && !GetDefault<UHeartGraphSettings>()->DisableAssetRegistryError)
	{
		FMessageLog("LoadErrors").Error()
			->AddToken(FTextToken::Create(FText::Format(
				LOCTEXT("MissingRuleForGraphNodeRegistrar", "Asset Manager does not have a rule for assets of type {0}. They will not be discoverable at runtime!"),
				FText::FromName(UGraphNodeRegistrar::StaticClass()->GetFName()))))
			->AddToken(FActionToken::Create(
				LOCTEXT("AddRuleForGraphNodeRegistrar", "Add entry to PrimaryAssetTypesToScan?"), FText::GetEmpty(),
				FOnActionTokenExecuted::CreateRaw(this, &FHeartEditorModule::AddRegistrarPrimaryAssetRule), true))
			->AddToken(FActionToken::Create(
				LOCTEXT("DisableGraphNodeRegistrarError", "Disable this error!"), FText::GetEmpty(),
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