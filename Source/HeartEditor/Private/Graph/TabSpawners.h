// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class SHeartPalette;

namespace Heart::AssetEditor
{
	class FAssetEditor;
	class SDetailsPanel;

	/** Callback for when a details panel is created */
	DECLARE_DELEGATE_OneParam(FOnDetailsCreated, const TSharedRef<SDetailsPanel>&);

	struct FDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FDetailsPanelSummoner(TSharedPtr<FAssetEditor> AssetEditor, const FOnDetailsCreated& InOnDetailsCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SDetailsPanel> DetailsPanel;
	};


	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FCreateGraphEditor, const FWorkflowTabSpawnInfo&)

	struct FGraphEditorSummoner : public FWorkflowTabFactory
	{
		static const FName TabId;

		FGraphEditorSummoner(TSharedPtr<FAssetEditor> AssetEditor, const FCreateGraphEditor& InCreateGraphEditor);

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

		FNodePaletteSummoner(TSharedPtr<FAssetEditor> AssetEditor, const FOnPaletteCreated& InOnPaletteCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SHeartPalette> HeartPalette;
	};
}
