#pragma once

// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	class FApplicationMode_PreviewCanvas : public FApplicationMode
	{
	public:
		static const FName ModeID;

		FApplicationMode_PreviewCanvas(TSharedRef<FHeartGraphEditor> AssetEditor);

		/** FApplicationMode interface */
		virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

		virtual void PreDeactivateMode() override {}
		virtual void PostActivateMode() override {}

	private:
		void OnDetailsCreated(const TSharedRef<IDetailsView>& InDetailsView);

	protected:
		/** The hosting app */
		TWeakPtr<FHeartGraphEditor> HeartGraphAssetEditorPtr;

		/** The tab factories we support */
		FWorkflowAllowedTabSet TabFactories;

	private:
		TSharedPtr<IDetailsView> DetailsView;
	};
}
