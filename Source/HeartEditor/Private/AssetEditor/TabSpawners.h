// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class SHeartPalette;

namespace Heart::AssetEditor
{
	//
	//		GRAPH EDITOR TABS
	//

	class FHeartGraphEditor;
	class SDetailsPanel;

	/** Callback for when a details panel is created */
	using FOnDetailsCreated = TDelegate<void(const TSharedRef<SDetailsPanel>&)>;

	struct FDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FDetailsPanelSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnDetailsCreated& InOnDetailsCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SDetailsPanel> DetailsPanel;
	};


	using FCreateGraphEditor = TDelegate<TSharedRef<SGraphEditor>(const FWorkflowTabSpawnInfo&)>;

	struct FGraphEditorSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FGraphEditorSummoner(const TSharedPtr<FHeartGraphEditor>& AssetEditor, const FCreateGraphEditor& InCreateGraphEditor);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		FCreateGraphEditor CreateGraphEditor;
	};


	/** Callback for when a node palette is created */
	using FOnPaletteCreated = TDelegate<void(const TSharedRef<SHeartPalette>&)>;

	struct FNodePaletteSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FNodePaletteSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnPaletteCreated& InOnPaletteCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SHeartPalette> HeartPalette;
	};
}