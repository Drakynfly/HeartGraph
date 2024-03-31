// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "AssetEditor/ApplicationMode_Editor.h"

#include "HeartEditorModule.h"
#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphAssetToolbar.h"

namespace Heart::AssetEditor
{
	const FLazyName FApplicationMode_Editor::ModeID("Heart_AssetEditorMode_Editor");

	FApplicationMode_Editor::FApplicationMode_Editor(TSharedRef<FHeartGraphEditor> AssetEditor)
		: FApplicationMode(ModeID)
	{
		HeartGraphAssetEditorPtr = AssetEditor;

		TabFactories.RegisterFactory(MakeShared<FDetailsPanelSummoner>(AssetEditor,
			FOnDetailsCreated::CreateSP(AssetEditor, &FHeartGraphEditor::OnDetailsPanelCreated)));

		TabFactories.RegisterFactory(MakeShared<FGraphEditorSummoner>(AssetEditor,
			FCreateGraphEditor::CreateSP(AssetEditor, &FHeartGraphEditor::CreateGraphWidget)));

		TabFactories.RegisterFactory(MakeShared<FNodePaletteSummoner>(AssetEditor,
			FOnPaletteCreated::CreateSP(AssetEditor, &FHeartGraphEditor::OnNodePaletteCreated)));

		TabLayout = FTabManager::NewLayout("HeartAssetEditor_Layout_v1")
			->AddArea
			(
				FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.225f)
						->AddTab(FDetailsPanelSummoner::TabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.65f)
						->AddTab(FGraphEditorSummoner::TabId, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.125f)
						->AddTab(FNodePaletteSummoner::TabId, ETabState::OpenedTab)
					)
			);

		FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
		ToolbarExtender = HeartEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders();
		AssetEditor->GetToolbar()->AddEditorModesToolbar(ToolbarExtender);
	}

	void FApplicationMode_Editor::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
	{
		const TSharedPtr<FHeartGraphEditor> AssetEditor = HeartGraphAssetEditorPtr.Pin();
		AssetEditor->RegisterTabSpawners(InTabManager.ToSharedRef());
		AssetEditor->PushTabFactories(TabFactories);
		FApplicationMode::RegisterTabFactories(InTabManager);
	}
}