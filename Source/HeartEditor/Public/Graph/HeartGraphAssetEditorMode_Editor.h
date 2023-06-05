// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

namespace Heart::AssetEditor
{
	class FAssetEditor;

	class FApplicationMode_Editor : public FApplicationMode
    {
    public:
    	FApplicationMode_Editor(TSharedRef<FAssetEditor> AssetEditor);

    	/** FApplicationMode interface */
    	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;

    protected:
    	/** The hosting app */
    	TWeakPtr<FAssetEditor> HeartGraphAssetEditorPtr;

    	/** The tab factories we support */
    	FWorkflowAllowedTabSet TabFactories;
    };
}