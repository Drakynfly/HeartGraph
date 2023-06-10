// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	class FApplicationMode_Editor : public FApplicationMode
    {
	public:
		static const FName ModeID;

    	FApplicationMode_Editor(TSharedRef<FHeartGraphEditor> AssetEditor);

    	/** FApplicationMode interface */
    	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

    protected:
    	/** The hosting app */
    	TWeakPtr<FHeartGraphEditor> HeartGraphAssetEditorPtr;

    	/** The tab factories we support */
    	FWorkflowAllowedTabSet TabFactories;
    };
}