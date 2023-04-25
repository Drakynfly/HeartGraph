// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetDefinition_HeartGraph.h"
#include "HeartEditorModule.h"

#include "Model/HeartGraph.h"

#include "Toolkits/IToolkit.h"
#include "HeartEditorShared.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartGraph"

FText UAssetDefinition_HeartGraph::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_HeartGraph", "Heart Graph");
}

FLinearColor UAssetDefinition_HeartGraph::GetAssetColor() const
{
	return FColor(255, 24, 44);
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
	const FHeartEditorModule& HeartModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");

	TArray<UHeartGraph*> HeartGraphs = OpenArgs.LoadObjects<UHeartGraph>();

	for (UHeartGraph* HeartGraph : HeartGraphs)
	{
		HeartModule.CreateHeartGraphAssetEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, HeartGraph);
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
