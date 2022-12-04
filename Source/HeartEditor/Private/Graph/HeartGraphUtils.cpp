// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphUtils.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraph.h"

#include "Model/HeartGraph.h"

#include "Toolkits/ToolkitManager.h"

TSharedPtr<FHeartGraphAssetEditor> FHeartGraphUtils::GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn)
{
	check(ObjectToFocusOn);

	TSharedPtr<FHeartGraphAssetEditor> HeartGraphAssetEditor;
	if (UHeartGraph* HeartGraph = Cast<const UHeartEdGraph>(ObjectToFocusOn)->GetHeartGraph())
	{
		const TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(HeartGraph);
		if (FoundAssetEditor.IsValid())
		{
			HeartGraphAssetEditor = StaticCastSharedPtr<FHeartGraphAssetEditor>(FoundAssetEditor);
		}
	}
	return HeartGraphAssetEditor;
}
