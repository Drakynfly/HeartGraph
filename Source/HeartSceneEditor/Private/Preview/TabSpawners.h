﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

//
//		SCENE PREVIEW TABS
//

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;
	class SPreviewSceneViewport;

	using FOnPreviewSceneCreated = TDelegate<void(const TSharedRef<SPreviewSceneViewport>&)>;

	struct FPreviewSceneSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FPreviewSceneSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnPreviewSceneCreated& OnPreviewSceneCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		TSharedPtr<SPreviewSceneViewport> PreviewViewport;
	};

	using FOnDetailsCreated = TDelegate<void(const TSharedRef<IDetailsView>&)>;

	struct FPreviewSceneDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FPreviewSceneDetailsPanelSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnDetailsCreated& OnDetailsCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		FOnDetailsCreated Callback;
	};
}