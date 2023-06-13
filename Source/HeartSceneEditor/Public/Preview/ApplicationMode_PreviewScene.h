// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;
	class SPreviewSceneViewport;

	class FApplicationMode_PreviewScene : public FApplicationMode
	{
	public:
		static const FName ModeID;

		FApplicationMode_PreviewScene(TSharedRef<FHeartGraphEditor> AssetEditor);

		/** FApplicationMode interface */
		virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

		virtual void PreDeactivateMode() override {}
		virtual void PostActivateMode() override {}

	private:
		void OnPreviewSceneCreated(const TSharedRef<SPreviewSceneViewport>& InViewport);
		void OnDetailsCreated(const TSharedRef<IDetailsView>& InDetailsView);

	protected:
		/** The hosting app */
		TWeakPtr<FHeartGraphEditor> HeartGraphAssetEditorPtr;

		/** The tab factories we support */
		FWorkflowAllowedTabSet TabFactories;

	private:
		TSharedPtr<SPreviewSceneViewport> Viewport;
		TSharedPtr<IDetailsView> DetailsView;
	};
}
