// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphUtils.h"

#include "Model/HeartGraph.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"

UHeartEdGraph::UHeartEdGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHeartEdGraph::PostLoad()
{
	Super::PostLoad();

	GetHeartGraph()->OnNodeCreatedInEditorExternally.BindUObject(this, &ThisClass::OnNodeCreatedInEditorExternally);
}

UEdGraph* UHeartEdGraph::CreateGraph(UHeartGraph* InHeartGraph)
{
	UHeartEdGraph* NewGraph = CastChecked<UHeartEdGraph>(FBlueprintEditorUtils::CreateNewGraph(InHeartGraph, NAME_None, StaticClass(), UHeartEdGraphSchema::StaticClass()));
	NewGraph->bAllowDeletion = false;

	InHeartGraph->HeartEdGraph = NewGraph;
	InHeartGraph->OnNodeCreatedInEditorExternally.BindUObject(NewGraph, &ThisClass::OnNodeCreatedInEditorExternally);

	NewGraph->GetSchema()->CreateDefaultNodesForGraph(*NewGraph);

	return NewGraph;
}

UHeartGraph* UHeartEdGraph::GetHeartGraph() const
{
	return CastChecked<UHeartGraph>(GetOuter());
}

void UHeartEdGraph::OnNodeCreatedInEditorExternally(UHeartGraphNode* Node)
{
	Modify();

	/*
	if (FromPin)
	{
		FromPin->Modify();
	}
	*/

	auto&& HeartGraph = GetHeartGraph();
	HeartGraph->Modify();

	const UClass* EdGraphNodeClass = UHeartEdGraphSchema::GetAssignedEdGraphNodeClass(Node->GetClass());
	auto&& NewEdGraphNode = NewObject<UHeartEdGraphNode>(this, EdGraphNodeClass, NAME_None, RF_NoFlags);
	NewEdGraphNode->CreateNewGuid();

	NewEdGraphNode->NodePosX = Node->GetLocation().X;
	NewEdGraphNode->NodePosY = Node->GetLocation().Y;

	// @todo the bUserAction thing might be true. what does that do if its true anyway?
	AddNode(NewEdGraphNode, false, false);

	// Assign nodes to each other
	// @todo can we avoid the first one. does the ed graph have to keep a reference to the runtime
	NewEdGraphNode->SetHeartGraphNode(Node);
	Node->SetEdGraphNode(NewEdGraphNode);

	//HeartGraph->AddNode(Node);

	NewEdGraphNode->PostPlacedNewNode();
	NewEdGraphNode->AllocateDefaultPins();

	NotifyGraphChanged();

	auto&& HeartGraphAssetEditor = Heart::GraphUtils::GetHeartGraphAssetEditor(this);
	if (HeartGraphAssetEditor.IsValid())
	{
		HeartGraphAssetEditor->SelectSingleNode(NewEdGraphNode);
	}

	HeartGraph->PostEditChange();
	HeartGraph->MarkPackageDirty();
}
