// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "AssetEditor/ApplicationMode_PreviewScene.h"

#include "HeartEditorModule.h"
#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphAssetToolbar.h"

#include "Preview/PreviewSceneCommands.h"

#define LOCTEXT_NAMESPACE "ApplicationMode_PreviewScene"

namespace Heart::AssetEditor
{
	FApplicationMode_PreviewScene::FApplicationMode_PreviewScene(TSharedRef<FHeartGraphEditor> AssetEditor)
	  : FApplicationMode(Modes::PreviewScene)
	{
		HeartGraphAssetEditorPtr = AssetEditor;

		TabFactories.RegisterFactory(MakeShared<FPreviewSceneSummoner>(AssetEditor));

		TabFactories.RegisterFactory(MakeShared<FPreviewSceneDetailsPanelSummoner>(AssetEditor));

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

		// @todo why doesnt this do anything?
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
}

#undef LOCTEXT_NAMESPACE