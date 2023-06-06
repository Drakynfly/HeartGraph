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
	DECLARE_DELEGATE_OneParam(FOnDetailsCreated, const TSharedRef<SDetailsPanel>&);

	struct FDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FDetailsPanelSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnDetailsCreated& InOnDetailsCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SDetailsPanel> DetailsPanel;
	};


	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FCreateGraphEditor, const FWorkflowTabSpawnInfo&)

	struct FGraphEditorSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FGraphEditorSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FCreateGraphEditor& InCreateGraphEditor);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		FCreateGraphEditor CreateGraphEditor;
	};


	/** Callback for when a node palette is created */
	DECLARE_DELEGATE_OneParam(FOnPaletteCreated, const TSharedRef<SHeartPalette>&);

	struct FNodePaletteSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FNodePaletteSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnPaletteCreated& InOnPaletteCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SHeartPalette> HeartPalette;
	};


	//
	//		SCENE PREVIEW TABS
	//

	struct FPreviewSceneSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FPreviewSceneSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<class SPreviewSceneViewport> PreviewViewport;
	};

	struct FPreviewSceneDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FPreviewSceneDetailsPanelSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	};
}
