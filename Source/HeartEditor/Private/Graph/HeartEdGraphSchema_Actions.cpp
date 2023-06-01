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
#include "HeartRegistryEditorSubsystem.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "HeartEdGraphSchema_Actions"

/////////////////////////////////////////////////////
// Heart Graph Node

UEdGraphNode* FHeartGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode /* = true*/)
{
	// prevent adding new nodes while playing
	if (GEditor->PlayWorld != nullptr)
	{
		return nullptr;
	}

	if (NodeSource.IsValid())
	{
		return CreateNode(ParentGraph, FromPin, NodeSource, Location, bSelectNewNode);
	}

	return nullptr;
}

UHeartEdGraphNode* FHeartGraphSchemaAction_NewNode::CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FHeartNodeSource NodeSource, const FVector2D Location, const bool bSelectNewNode /*= true*/)
{
	check(NodeSource.IsValid());
	if (!ensure(GEditor)) return nullptr;

	auto&& EditorSubsystem = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>();
	check(EditorSubsystem);


	/**-----------------------------*/
	/*		Prepare Heart Graph		*/
	/**-----------------------------*/

	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));

	ParentGraph->Modify();
	if (FromPin)
	{
		FromPin->Modify();
	}

	auto&& HeartGraph = CastChecked<UHeartEdGraph>(ParentGraph)->GetHeartGraph();
	HeartGraph->Modify();


	/**-----------------------------*/
	/*		Create Runtime Node		*/
	/**-----------------------------*/

	UHeartGraphNode* NewGraphNode;
	if (const UClass* AsClass = NodeSource.As<UClass>())
	{
		NewGraphNode = HeartGraph->CreateNodeFromClass(AsClass, Location);
	}
	else
	{
		NewGraphNode = HeartGraph->CreateNodeFromObject(NodeSource.As<UObject>(), Location);
	}
	check(NewGraphNode)


	/**-----------------------------*/
	/*		Create EdGraphNode		*/
	/**-----------------------------*/

	const UClass* EdGraphNodeClass = EditorSubsystem->GetAssignedEdGraphNodeClass(NewGraphNode->GetClass());
	UHeartEdGraphNode* NewEdGraphNode = NewObject<UHeartEdGraphNode>(ParentGraph, EdGraphNodeClass, NAME_None,
																	   RF_Transactional);
	NewEdGraphNode->CreateNewGuid();

	NewEdGraphNode->NodePosX = Location.X;
	NewEdGraphNode->NodePosY = Location.Y;
	//ParentGraph->AddNode(NewEdGraphNode, false, bSelectNewNode);

	// Assign nodes to each other
	// @todo can we avoid the first one. does the ed graph have to keep a reference to the runtime
	NewEdGraphNode->SetHeartGraphNode(NewGraphNode);
	NewGraphNode->SetEdGraphNode(NewEdGraphNode);

	HeartGraph->AddNode(NewGraphNode);

	NewEdGraphNode->PostPlacedNewNode();
	NewEdGraphNode->AllocateDefaultPins();

	NewEdGraphNode->AutowireNewNode(FromPin);

	if (bSelectNewNode)
	{
		auto&& HeartGraphAssetEditor = Heart::GraphUtils::GetHeartGraphAssetEditor(ParentGraph);
		if (HeartGraphAssetEditor.IsValid())
		{
			HeartGraphAssetEditor->SelectSingleNode(NewEdGraphNode);
		}
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
		Heart::GraphUtils::GetHeartGraphAssetEditor(ParentGraph)->PasteNodesHere(Location);
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
	if (Heart::GraphUtils::GetHeartGraphAssetEditor(ParentGraph)->GetBoundsForSelectedNodes(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation);
}

#undef LOCTEXT_NAMESPACE
