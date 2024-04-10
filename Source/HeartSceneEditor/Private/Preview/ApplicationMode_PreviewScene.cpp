// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/ApplicationMode_PreviewScene.h"

#include "HeartEditorModule.h"
#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphAssetToolbar.h"
#include "Preview/HeartPreviewScene.h"
#include "Preview/PreviewSceneConfig.h"

#include "Preview/PreviewSceneCommands.h"
#include "Preview/SPreviewSceneViewport.h"

#define LOCTEXT_NAMESPACE "ApplicationMode_PreviewScene"

namespace Heart::AssetEditor
{
	const FLazyName FApplicationMode_PreviewScene::ModeID("Heart_AssetEditorMode_PreviewScene");

	FApplicationMode_PreviewScene::FApplicationMode_PreviewScene(TSharedRef<FHeartGraphEditor> AssetEditor)
	  : FApplicationMode(ModeID)
	{
		HeartGraphAssetEditorPtr = AssetEditor;

		TabFactories.RegisterFactory(MakeShared<FPreviewSceneSummoner>(AssetEditor,
			FOnPreviewSceneCreated::CreateRaw(this, &FApplicationMode_PreviewScene::OnPreviewSceneCreated)));

		TabFactories.RegisterFactory(MakeShared<FPreviewSceneDetailsPanelSummoner>(AssetEditor,
			FOnDetailsCreated::CreateRaw(this, &FApplicationMode_PreviewScene::OnDetailsCreated)));

		TabLayout = FTabManager::NewLayout("HeartAssetEditor_PreviewScene_Layout_v0.1")
			->AddArea
			(
				FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewSplitter()
					->SetSizeCoefficient(0.6f)
					->SetOrientation(Orient_Horizontal)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->AddTab(FPreviewSceneSummoner::TabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->AddTab(FPreviewSceneDetailsPanelSummoner::TabId, ETabState::OpenedTab)
					)
				)
			);

		struct Local
		{
			static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
			{
				ToolbarBuilder.BeginSection("PreviewScene");
				{
					ToolbarBuilder.AddToolBarButton(
						FPreviewSceneCommands::Get().FocusViewport,
						NAME_None,
						LOCTEXT("FocusViewport", "Focus Viewport"),
						LOCTEXT("FocusViewportTooltip", "Focuses Viewport on selected Mesh"),
						FSlateIcon()
					);
				}
				ToolbarBuilder.EndSection();
			}
		};

		FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
		ToolbarExtender = HeartEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders();
		AssetEditor->GetToolbar()->AddEditorModesToolbar(ToolbarExtender);

		// @todo why doesn't this do anything?
		ToolbarExtender->AddToolBarExtension(
			"PreviewScene",
			EExtensionHook::After,
			AssetEditor->GetToolkitCommands(),
			FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar));
	}

	void FApplicationMode_PreviewScene::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
	{
		const TSharedPtr<FHeartGraphEditor> AssetEditor = HeartGraphAssetEditorPtr.Pin();
		AssetEditor->RegisterTabSpawners(InTabManager.ToSharedRef());
		AssetEditor->PushTabFactories(TabFactories);
		FApplicationMode::RegisterTabFactories(InTabManager);
	}

	void FApplicationMode_PreviewScene::OnPreviewSceneCreated(const TSharedRef<SPreviewSceneViewport>& InViewport)
	{
		Viewport = InViewport;
	}

	void FApplicationMode_PreviewScene::OnDetailsCreated(const TSharedRef<IDetailsView>& InDetailsView)
	{
		DetailsView = InDetailsView;

		if (UPreviewSceneConfig* Config = Viewport->GetPreviewScene()->GetConfig())
		{
			DetailsView->SetObject(Config);
		}
	}
}

#undef LOCTEXT_NAMESPACE