// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetEditorMode_Editor.h"

#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"

namespace Heart::AssetEditor
{
	FApplicationMode_Editor::FApplicationMode_Editor(TSharedRef<FAssetEditor> AssetEditor)
		: FApplicationMode(Modes::Editor)
	{
		HeartGraphAssetEditorPtr = AssetEditor;

		TabFactories.RegisterFactory(MakeShared<FDetailsPanelSummoner>(AssetEditor,
			FOnDetailsCreated::CreateSP(AssetEditor, &FAssetEditor::OnDetailsPanelCreated)));

		TabFactories.RegisterFactory(MakeShared<FGraphEditorSummoner>(AssetEditor,
			FCreateGraphEditor::CreateSP(AssetEditor, &FAssetEditor::CreateGraphWidget)));

		TabFactories.RegisterFactory(MakeShared<FNodePaletteSummoner>(AssetEditor,
			FOnPaletteCreated::CreateSP(AssetEditor, &FAssetEditor::OnNodePaletteCreated)));

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
	}

	void FApplicationMode_Editor::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
	{
		const TSharedPtr<FAssetEditor> AssetEditor = HeartGraphAssetEditorPtr.Pin();
		AssetEditor->RegisterTabSpawners(InTabManager.ToSharedRef());
		AssetEditor->PushTabFactories(TabFactories);
		FApplicationMode::RegisterTabFactories(InTabManager);
	}
}
