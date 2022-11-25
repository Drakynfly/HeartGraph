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

	const UClass* GraphNodeClass = UHeartEdGraphSchema::GetAssignedGraphNodeClass(NodeClass);
	auto&& NewGraphNode = NewObject<UHeartEdGraphNode>(ParentGraph, GraphNodeClass, NAME_None, RF_Transactional);
	NewGraphNode->CreateNewGuid();

	NewGraphNode->NodePosX = Location.X;
	NewGraphNode->NodePosY = Location.Y;
	ParentGraph->AddNode(NewGraphNode, false, bSelectNewNode);

	FVector2D NodeLocation = FVector2D(NewGraphNode->NodePosX, NewGraphNode->NodePosY);
	auto&& NewNode = HeartGraph->CreateNode(NodeClass, NodeLocation);
	NewGraphNode->SetHeartGraphNode(NewNode);

	NewGraphNode->PostPlacedNewNode();
	NewGraphNode->AllocateDefaultPins();

	NewGraphNode->AutowireNewNode(FromPin);

	ParentGraph->NotifyGraphChanged();

	auto&& HeartGraphAssetEditor = FHeartGraphUtils::GetHeartGraphAssetEditor(ParentGraph);
	if (HeartGraphAssetEditor.IsValid())
	{
		HeartGraphAssetEditor->SelectSingleNode(NewGraphNode);
	}

	HeartGraph->PostEditChange();
	HeartGraph->MarkPackageDirty();

	return NewGraphNode;
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
