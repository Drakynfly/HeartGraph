// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetTypeActions_HeartGraph.h"
//#include "Asset/SHeartDiff.h"
#include "HeartEditorModule.h"

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
			const FHeartEditorModule* HeartModule = &FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
			HeartModule->CreateHeartGraphAssetEditor(Mode, EditWithinLevelEditor, HeartGraph);
		}
	}
}

void FAssetTypeActions_HeartGraph::PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const FRevisionInfo& OldRevision, const FRevisionInfo& NewRevision) const
{
	const UHeartGraph* OldHeartGraph = CastChecked<UHeartGraph>(OldAsset);
	const UHeartGraph* NewHeartGraph = CastChecked<UHeartGraph>(NewAsset);

	// sometimes we're comparing different revisions of one single asset (other
	// times we're comparing two completely separate assets altogether)
	const bool bIsSingleAsset = (OldHeartGraph->GetName() == NewHeartGraph->GetName());

	static const FText BasicWindowTitle = LOCTEXT("HeartGraphDiff", "HeartGraph Diff");
	const FText WindowTitle = !bIsSingleAsset ? BasicWindowTitle : FText::Format(LOCTEXT("HeartGraph Diff", "{0} - HeartGraph Diff"), FText::FromString(NewHeartGraph->GetName()));

	//SHeartDiff::CreateDiffWindow(WindowTitle, OldHeartGraph, NewHeartGraph, OldRevision, NewRevision);
}

#undef LOCTEXT_NAMESPACE
