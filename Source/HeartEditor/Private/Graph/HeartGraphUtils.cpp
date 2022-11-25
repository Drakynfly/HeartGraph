// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphUtils.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraph.h"

#include "Model/HeartGraph.h"

#include "Toolkits/ToolkitManager.h"

TSharedPtr<FHeartGraphAssetEditor> FHeartGraphUtils::GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn)
{
	check(ObjectToFocusOn);

	TSharedPtr<FHeartGraphAssetEditor> FlowAssetEditor;
	if (UHeartGraph* FlowAsset = Cast<const UHeartEdGraph>(ObjectToFocusOn)->GetHeartGraph())
	{
		const TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(FlowAsset);
		if (FoundAssetEditor.IsValid())
		{
			FlowAssetEditor = StaticCastSharedPtr<FHeartGraphAssetEditor>(FoundAssetEditor);
		}
	}
	return FlowAssetEditor;
}
