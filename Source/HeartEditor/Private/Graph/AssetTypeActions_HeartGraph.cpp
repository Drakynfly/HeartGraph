// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetTypeActions_HeartGraph.h"
#include "HeartEditorModule.h"
#include "Graph/HeartGraphUtils.h"

#include "Model/HeartGraph.h"

#include "Toolkits/IToolkit.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_HeartGraph"

FText FAssetTypeActions_HeartGraph::GetName() const
{
	return LOCTEXT("AssetTypeActions_HeartGraph", "Heart Graph");
}

uint32 FAssetTypeActions_HeartGraph::GetCategories()
{
	return FHeartEditorModule::HeartAssetCategory;
}

UClass* FAssetTypeActions_HeartGraph::GetSupportedClass() const
{
	return UHeartGraph::StaticClass();
}

void FAssetTypeActions_HeartGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UHeartGraph* HeartGraph = Cast<UHeartGraph>(*ObjIt))
		{
			Heart::GraphUtils::CreateHeartGraphAssetEditor(Mode, EditWithinLevelEditor, HeartGraph);
		}
	}
}

#undef LOCTEXT_NAMESPACE
