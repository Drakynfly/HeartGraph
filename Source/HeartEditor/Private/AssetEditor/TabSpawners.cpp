// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/Widgets/SHeartDetailsPanel.h"
#include "Graph/Widgets/SHeartPalette.h"

#define LOCTEXT_NAMESPACE "Heart::AssetEditor::TabSpawners"

namespace Heart::AssetEditor
{
	/*-------------------
		DETAILS PANEL
	 -------------------*/

	const FLazyName FDetailsPanelSummoner::TabId("Heart_AssetEditor_Details");

	FDetailsPanelSummoner::FDetailsPanelSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor,
												 const FOnDetailsCreated& InOnDetailsCreated)
	  : FWorkflowTabFactory(TabId, AssetEditor)
	{
		TabLabel = LOCTEXT("DetailsTabLabel", "Details");
    	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");
    	bIsSingleton = true;

    	ViewMenuDescription = LOCTEXT("DetailsPanel_Description", "Details Panel");
    	ViewMenuTooltip = LOCTEXT("DetailsPanel_ToolTip", "Shows the details panel for the graph or selected nodes");

		DetailsPanel = SNew(SDetailsPanel, AssetEditor);
		InOnDetailsCreated.Execute(DetailsPanel.ToSharedRef());
	}

	TSharedRef<SWidget> FDetailsPanelSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		return DetailsPanel.ToSharedRef();
	}

	FText FDetailsPanelSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("DetailsPanel_TabTooltip", "Details panel for graph, or selected graph node and it's node object");
	}


	/*-------------------
		 GRAPH EDITOR
	 -------------------*/

	const FLazyName FGraphEditorSummoner::TabId("Heart_AssetEditor_GraphEditor");

	FGraphEditorSummoner::FGraphEditorSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FCreateGraphEditor& InCreateGraphEditor)
	  : FWorkflowTabFactory(TabId, AssetEditor),
		CreateGraphEditor(InCreateGraphEditor)
	{
		TabLabel = LOCTEXT("GraphTabLabel", "Graph");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("GraphEditor_Description", "");
		ViewMenuTooltip = LOCTEXT("GraphEditor_ToolTip", "");
	}

	TSharedRef<SWidget> FGraphEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		return CreateGraphEditor.Execute(Info);
	}

	FText FGraphEditorSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("GraphEditor_TabTooltip", "Node graph editor for the Heart Graph Asset");
	}


	/*-------------------
		 NODE PALETTE
	 -------------------*/

	const FLazyName FNodePaletteSummoner::TabId("Heart_AssetEditor_NodePalette");

	FNodePaletteSummoner::FNodePaletteSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnPaletteCreated& InOnPaletteCreated)
	  : FWorkflowTabFactory(TabId, AssetEditor)
	{
		TabLabel = LOCTEXT("NodePaletteTabLabel", "Palette");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.Palette");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("NodePalette_Description", "");
		ViewMenuTooltip = LOCTEXT("NodePalette_ToolTip", "");

		HeartPalette = SNew(SHeartPalette, AssetEditor);
		InOnPaletteCreated.Execute(HeartPalette.ToSharedRef());
	}

	TSharedRef<SWidget> FNodePaletteSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		return HeartPalette.ToSharedRef();
	}

	FText FNodePaletteSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("NodePalette_TabTooltip", "");
	}
}

#undef LOCTEXT_NAMESPACE