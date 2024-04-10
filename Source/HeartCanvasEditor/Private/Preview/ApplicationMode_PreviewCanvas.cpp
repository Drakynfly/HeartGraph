// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/ApplicationMode_PreviewCanvas.h"

#include "HeartEditorModule.h"
#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphAssetToolbar.h"

#define LOCTEXT_NAMESPACE "ApplicationMode_PreviewCanvas"

namespace Heart::AssetEditor
{
	const FLazyName FApplicationMode_PreviewCanvas::ModeID("Heart_AssetEditorMode_PreviewCanvas");

	FApplicationMode_PreviewCanvas::FApplicationMode_PreviewCanvas(TSharedRef<FHeartGraphEditor> AssetEditor)
	  : FApplicationMode(ModeID)
	{
		HeartGraphAssetEditorPtr = AssetEditor;

		TabFactories.RegisterFactory(MakeShared<FPreviewCanvasSummoner>(AssetEditor));

		TabFactories.RegisterFactory(MakeShared<FPreviewCanvasDetailsPanelSummoner>(AssetEditor,
			FOnDetailsCreated::CreateRaw(this, &FApplicationMode_PreviewCanvas::OnDetailsCreated)));

		TabLayout = FTabManager::NewLayout("HeartAssetEditor_PreviewCanvas_Layout_v0.1")
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
						->AddTab(FPreviewCanvasSummoner::TabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->AddTab(FPreviewCanvasDetailsPanelSummoner::TabId, ETabState::OpenedTab)
					)
				)
			);

		FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
		ToolbarExtender = HeartEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders();
		AssetEditor->GetToolbar()->AddEditorModesToolbar(ToolbarExtender);
	}

	void FApplicationMode_PreviewCanvas::RegisterTabFactories(const TSharedPtr<FTabManager> InTabManager)
	{
		const TSharedPtr<FHeartGraphEditor> AssetEditor = HeartGraphAssetEditorPtr.Pin();
		AssetEditor->RegisterTabSpawners(InTabManager.ToSharedRef());
		AssetEditor->PushTabFactories(TabFactories);
		FApplicationMode::RegisterTabFactories(InTabManager);
	}

	void FApplicationMode_PreviewCanvas::OnDetailsCreated(const TSharedRef<IDetailsView>& InDetailsView)
	{
		DetailsView = InDetailsView;
	}
}

#undef LOCTEXT_NAMESPACE