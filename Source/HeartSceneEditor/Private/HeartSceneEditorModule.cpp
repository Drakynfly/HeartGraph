// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneEditorModule.h"

#include "HeartEditorModule.h"
#include "HeartSceneExtension.h"
#include "Model/HeartGraph.h"
#include "Preview/ApplicationMode_PreviewScene.h"

#define LOCTEXT_NAMESPACE "HeartSceneEditorModule"

void FHeartSceneEditorModule::StartupModule()
{
	using namespace Heart::AssetEditor;

	// Register 3D scene preview application mode
	FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");

	FRegisteredApplicationMode PreviewScene;
	PreviewScene.LocalizedName = LOCTEXT("ApplicationMode_PreviewScene.LocalizedName", "Scene");
	PreviewScene.TooltipText = LOCTEXT("ApplicationMode_PreviewScene.TooltipText", "Switch to Preview Scene Mode");
	PreviewScene.CreateModeInstance.BindLambda(
		[](const TSharedRef<FHeartGraphEditor>& Editor)
		{
			return MakeShareable(new FApplicationMode_PreviewScene(Editor));
		});
	PreviewScene.SupportsAsset.BindLambda(
		[](const UHeartGraph* Asset)
		{
			return IsValid(Asset->GetExtension<UHeartSceneExtension>());
		});

	HeartEditorModule.RegisterApplicationMode(FApplicationMode_PreviewScene::ModeID, PreviewScene);
}

void FHeartSceneEditorModule::ShutdownModule()
{
	// Remove registered application mode
	FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
	HeartEditorModule.DeregisterApplicationMode(Heart::AssetEditor::FApplicationMode_PreviewScene::ModeID);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartSceneEditorModule, HeartSceneEditor)