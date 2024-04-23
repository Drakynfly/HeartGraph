// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraphSchema_Actions.h"

#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphUtils.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Model/HeartGraph.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphNode_Comment.h"
#include "Editor.h"
#include "ScopedTransaction.h"
#include "Input/EdGraphPointerWrappers.h"
#include "Input/HeartSlateInputLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEdGraphSchema_Actions)

#define LOCTEXT_NAMESPACE "HeartEdGraphSchema_Actions"

// Heart Graph Node

UEdGraphNode* FHeartGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode /* = true*/)
{
	// prevent adding new nodes while playing
	if (GEditor->PlayWorld != nullptr)
	{
		return nullptr;
	}

	if (ensure(Archetype.Source.IsValid() && IsValid(Archetype.GraphNode)))
	{
		return CreateNode(ParentGraph, FromPin, Archetype, Location, bSelectNewNode);
	}

	return nullptr;
}

UHeartEdGraphNode* FHeartGraphSchemaAction_NewNode::CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FHeartNodeArchetype Archetype, const FVector2D Location, const bool bSelectNewNode /*= true*/)
{
	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));

	ParentGraph->Modify();

	auto&& HeartEdGraph = CastChecked<UHeartEdGraph>(ParentGraph);
	auto&& HeartGraph = HeartEdGraph->GetHeartGraph();
	HeartGraph->Modify();

	UHeartGraphNode* NewGraphNode;

	// @todo this would not work, in the rare edge case of making a node with a UClass as a referenced source
	if (const UClass* AsClass = Archetype.Source.As<UClass>())
	{
		NewGraphNode = HeartGraph->CreateNode_Instanced(Archetype.GraphNode, AsClass, Location);
	}
	else
	{
		NewGraphNode = HeartGraph->CreateNode_Reference(Archetype.GraphNode, Archetype.Source.As<UObject>(), Location);
	}
	check(NewGraphNode)

	// Add runtime node to graph, this will trigger the EdGraphNode to be created by in UHeartEdGraph::CreateEdGraphNode
	HeartGraph->AddNode(NewGraphNode);

	auto&& HeartEdGraphNode = HeartEdGraph->FindEdGraphNodeForNode(NewGraphNode);
	if (!IsValid(HeartEdGraphNode))
	{
		// Failed to create EdGraphNode, bail
		return nullptr;
	}

	// Connect up to editor pin. @todo will this be handled automatically by runtime eventually?
	if (FromPin)
	{
		FromPin->Modify();
		HeartEdGraphNode->AutowireNewNode(FromPin);
	}

	auto&& HeartGraphAssetEditor = Heart::GraphUtils::GetHeartGraphAssetEditor(ParentGraph);
	if (HeartGraphAssetEditor.IsValid())
	{
		HeartGraphAssetEditor->SelectSingleNode(HeartEdGraphNode);
	}

	HeartGraph->PostEditChange();

	return HeartEdGraphNode;
}

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

UEdGraphNode* FHeartGraphSchemaAction_LinkerBinding::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, const bool bSelectNewNode)
{
	UHeartEdGraph* EdGraph = CastChecked<UHeartEdGraph>(ParentGraph);

	if (UHeartSlateInputLinker* Linker = EdGraph->GetEditorLinker())
	{
		UObject* Target;

		if (IsValid(ContextNode))
		{
			Target = ContextNode;
		}
		else if (FromPin)
		{
			Target = UHeartEdGraphPin::Wrap(FromPin);
		}
		else
		{
			Target = EdGraph;
		}

		FHeartManualEvent Event;
		Event.EventValue = 1.0;

		Linker->HandleManualInput(Target, Key, Event);
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE