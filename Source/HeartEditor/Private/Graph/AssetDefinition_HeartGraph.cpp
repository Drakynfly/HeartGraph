// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetDefinition_HeartGraph.h"
#include "Graph/HeartEdGraphUtils.h"

#include "Model/HeartGraph.h"

#include "Toolkits/IToolkit.h"
#include "HeartEditorShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_HeartGraph)

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartGraph"

FText UAssetDefinition_HeartGraph::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_HeartGraph", "Heart Graph");
}

FLinearColor UAssetDefinition_HeartGraph::GetAssetColor() const
{
	return Heart::EditorShared::HeartColor;
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HeartGraph::GetAssetCategories() const
{
	return Heart::EditorShared::GetAssetCategories();
}

TSoftClassPtr<> UAssetDefinition_HeartGraph::GetAssetClass() const
{
	return UHeartGraph::StaticClass();
}

EAssetCommandResult UAssetDefinition_HeartGraph::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	TArray<UHeartGraph*> HeartGraphs = OpenArgs.LoadObjects<UHeartGraph>();

	for (UHeartGraph* HeartGraph : HeartGraphs)
	{
		Heart::GraphUtils::CreateHeartGraphAssetEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, HeartGraph);
	}

	return EAssetCommandResult::Handled;
}

EAssetCommandResult UAssetDefinition_HeartGraph::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	if (DiffArgs.OldAsset == nullptr && DiffArgs.NewAsset == nullptr)
	{
		return EAssetCommandResult::Unhandled;
	}

	const UHeartGraph* OldHeartGraph = CastChecked<UHeartGraph>(DiffArgs.OldAsset);
	const UHeartGraph* NewHeartGraph = CastChecked<UHeartGraph>(DiffArgs.NewAsset);

	// sometimes we're comparing different revisions of one single asset (other
	// times we're comparing two completely separate assets altogether)
	const bool bIsSingleAsset = (OldHeartGraph->GetName() == NewHeartGraph->GetName());

	static const FText BasicWindowTitle = LOCTEXT("HeartGraphDiff", "HeartGraph Diff");
	const FText WindowTitle = !bIsSingleAsset ? BasicWindowTitle : FText::Format(LOCTEXT("HeartGraph Diff", "{0} - HeartGraph Diff"), FText::FromString(NewHeartGraph->GetName()));

	//SHeartDiff::CreateDiffWindow(WindowTitle, OldHeartGraph, NewHeartGraph, OldRevision, NewRevision);
	//return EAssetCommandResult::Handled;
	return EAssetCommandResult::Unhandled;
}

#undef LOCTEXT_NAMESPACE