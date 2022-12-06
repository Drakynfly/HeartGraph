// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraphSchema_Actions.h"

#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartGraphUtils.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Model/HeartGraph.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphNode_Comment.h"
#include "Editor.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "HeartGraphSchema_Actions"

/////////////////////////////////////////////////////
// Heart Graph Node

UEdGraphNode* FHeartGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /* = true*/)
{
	// prevent adding new nodes while playing
	if (GEditor->PlayWorld != nullptr)
	{
		return nullptr;
	}

	if (NodeClass)
	{
		return CreateNode(ParentGraph, FromPin, NodeClass, Location, bSelectNewNode);
	}

	return nullptr;
}

UHeartEdGraphNode* FHeartGraphSchemaAction_NewNode::CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, UClass* NodeClass, const FVector2D Location, const bool bSelectNewNode /*= true*/)
{
	check(NodeClass);

	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));

	ParentGraph->Modify();
	if (FromPin)
	{
		FromPin->Modify();
	}

	auto&& HeartGraph = CastChecked<UHeartEdGraph>(ParentGraph)->GetHeartGraph();
	HeartGraph->Modify();

	const UClass* EdGraphNodeClass = UHeartEdGraphSchema::GetAssignedEdGraphNodeClass(NodeClass);
	auto&& NewEdGraphNode = NewObject<UHeartEdGraphNode>(ParentGraph, EdGraphNodeClass, NAME_None, RF_Transactional);
	NewEdGraphNode->CreateNewGuid();

	NewEdGraphNode->NodePosX = Location.X;
	NewEdGraphNode->NodePosY = Location.Y;
	//ParentGraph->AddNode(NewEdGraphNode, false, bSelectNewNode);

	const FVector2D NodeLocation = FVector2D(NewEdGraphNode->NodePosX, NewEdGraphNode->NodePosY);
	auto&& NewGraphNode = HeartGraph->CreateNodeFromClass(NodeClass, NodeLocation);

	// Assign nodes to each other
	// @todo can we avoid the first one. does the ed graph have to keep a reference to the runtime
	NewEdGraphNode->SetHeartGraphNode(NewGraphNode);
	NewGraphNode->SetEdGraphNode(NewEdGraphNode);

	HeartGraph->AddNode(NewGraphNode);

	NewEdGraphNode->PostPlacedNewNode();
	NewEdGraphNode->AllocateDefaultPins();

	NewEdGraphNode->AutowireNewNode(FromPin);

	ParentGraph->NotifyGraphChanged();

	auto&& HeartGraphAssetEditor = FHeartGraphUtils::GetHeartGraphAssetEditor(ParentGraph);
	if (HeartGraphAssetEditor.IsValid())
	{
		HeartGraphAssetEditor->SelectSingleNode(NewEdGraphNode);
	}

	HeartGraph->PostEditChange();
	HeartGraph->MarkPackageDirty();

	return NewEdGraphNode;
}

/////////////////////////////////////////////////////
// Paste Node

UEdGraphNode* FHeartGraphSchemaAction_Paste::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// prevent adding new nodes while playing
	if (GEditor->PlayWorld == nullptr)
	{
		FHeartGraphUtils::GetHeartGraphAssetEditor(ParentGraph)->PasteNodesHere(Location);
	}

	return nullptr;
}

/////////////////////////////////////////////////////
// Comment Node

UEdGraphNode* FHeartGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// prevent adding new nodes while playing
	if (GEditor->PlayWorld != nullptr)
	{
		return nullptr;
	}

	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();
	FVector2D SpawnLocation = Location;

	FSlateRect Bounds;
	if (FHeartGraphUtils::GetHeartGraphAssetEditor(ParentGraph)->GetBoundsForSelectedNodes(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation);
}

#undef LOCTEXT_NAMESPACE
