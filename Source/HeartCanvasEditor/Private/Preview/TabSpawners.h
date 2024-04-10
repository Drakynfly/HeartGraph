﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

//
//		SCENE PREVIEW TABS
//

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	struct FPreviewCanvasSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FPreviewCanvasSummoner(const TSharedPtr<FHeartGraphEditor>& AssetEditor);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	};

	using FOnDetailsCreated = TDelegate<void(const TSharedRef<IDetailsView>&)>;

	struct FPreviewCanvasDetailsPanelSummoner : public FWorkflowTabFactory
	{
		static const FLazyName TabId;

		FPreviewCanvasDetailsPanelSummoner(const TSharedPtr<FHeartGraphEditor>& AssetEditor, const FOnDetailsCreated& OnDetailsCreated);

		virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
		virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	private:
		FOnDetailsCreated Callback;
	};
}