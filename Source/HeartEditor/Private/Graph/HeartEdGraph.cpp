// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraph.h"

#include "HeartRegistryEditorSubsystem.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartEdGraphUtils.h"
#include "Graph/HeartGraphAssetEditor.h"

#include "Model/HeartGraph.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEdGraph)

UHeartEdGraph::UHeartEdGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHeartEdGraph::PostInitProperties()
{
	Super::PostInitProperties();

	if (!IsTemplate())
	{
		UHeartGraph* HeartGraph = GetHeartGraph();

		HeartGraph->OnNodeAdded.AddUObject(this, &ThisClass::OnNodeAdded);
		HeartGraph->OnNodeRemoved.AddUObject(this, &ThisClass::OnNodeRemoved);
		HeartGraph->OnNodeConnectionsChanged.AddUObject(this, &ThisClass::OnNodeConnectionsChanged);
	}
}

void UHeartEdGraph::PostLoad()
{
	Super::PostLoad();

	UHeartGraph* HeartGraph = GetHeartGraph();

	for (auto&& Element : HeartGraph->Nodes)
	{
		if (IsValid(Element.Value))
		{
			// For various reasons, runtime nodes could be missing a EdGraph equivalent, and we want to silently repair these,
			// or these nodes will be invisible in the EdGraph
			if (!IsValid(FindEdGraphNodeForNode(Element.Value)))
			{
				// Broadcasting this delegate is our hook to request the EdGraph to generate an EdGraphNode for us.
				OnNodeCreatedInEditorExternally(Element.Value);
			}
		}
	}
}

UEdGraph* UHeartEdGraph::CreateGraph(UHeartGraph* InHeartGraph)
{
	UHeartEdGraph* NewGraph = CastChecked<UHeartEdGraph>(FBlueprintEditorUtils::CreateNewGraph(InHeartGraph, NAME_None, StaticClass(), UHeartEdGraphSchema::StaticClass()));
	NewGraph->bAllowDeletion = false;

	InHeartGraph->HeartEdGraph = NewGraph;
	for (auto&& Element : InHeartGraph->Nodes)
	{
		if (IsValid(Element.Value))
		{
			NewGraph->OnNodeCreatedInEditorExternally(Element.Value);
		}
	}

	NewGraph->GetSchema()->CreateDefaultNodesForGraph(*NewGraph);

	return NewGraph;
}

UHeartEdGraphNode* UHeartEdGraph::FindEdGraphNode(const TFunction<bool(const UHeartEdGraphNode*)>& Iter)
{
	for (auto Element : Nodes)
	{
		if (UHeartEdGraphNode* HeartEdGraphNode = Cast<UHeartEdGraphNode>(Element))
		{
			if (Iter(HeartEdGraphNode))
			{
				return HeartEdGraphNode;
			}
		}
	}
	return nullptr;
}

UHeartEdGraphNode* UHeartEdGraph::FindEdGraphNodeForNode(const UHeartGraphNode* HeartGraphNode)
{
	return FindEdGraphNode(
		[HeartGraphNode](const UHeartEdGraphNode* Node)
		{
			return Node->GetHeartGraphNode() == HeartGraphNode;
		});
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

	if (!ensure(GEditor)) return;

	const UClass* EdGraphNodeClass = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>()->GetAssignedEdGraphNodeClass(Node->GetClass());
	auto&& NewEdGraphNode = NewObject<UHeartEdGraphNode>(this, EdGraphNodeClass, NAME_None, RF_NoFlags);
	NewEdGraphNode->CreateNewGuid();

	NewEdGraphNode->NodePosX = Node->GetLocation().X;
	NewEdGraphNode->NodePosY = Node->GetLocation().Y;

	// @todo the bUserAction thing might be true. what does that do if its true anyway?
	AddNode(NewEdGraphNode, false, false);

	// Assign nodes to each other
	// @todo can we avoid the first one. does the ed graph have to keep a reference to the runtime
	NewEdGraphNode->SetHeartGraphNode(Node);

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

void UHeartEdGraph::OnNodeAdded(UHeartGraphNode* HeartGraphNode)
{
	if (IsValid(HeartGraphNode))
	{
		if (auto&& EdGraphNode = FindEdGraphNodeForNode(HeartGraphNode))
		{
			AddNode(EdGraphNode);
		}
		else
		{
			OnNodeCreatedInEditorExternally(HeartGraphNode);
		}
	}
}

void UHeartEdGraph::OnNodeRemoved(UHeartGraphNode* HeartGraphNode)
{
	if (IsValid(HeartGraphNode))
	{
		if (auto&& EdGraphNode = FindEdGraphNodeForNode(HeartGraphNode))
		{
			RemoveNode(EdGraphNode);
		}
	}
}

void UHeartEdGraph::OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& HeartGraphConnectionEvent)
{
	if (HeartGraphConnectionEvent.AffectedNodes.Num() != 2 ||
		HeartGraphConnectionEvent.AffectedPins.Num() != 2)
	{
		return;
	}

	const UHeartGraphNode* NodeA = HeartGraphConnectionEvent.AffectedNodes.Get(FSetElementId::FromInteger(0));
	const UHeartGraphNode* NodeB = HeartGraphConnectionEvent.AffectedNodes.Get(FSetElementId::FromInteger(1));
	const UEdGraphNode* EdNodeA = FindEdGraphNodeForNode(NodeA);
	const UEdGraphNode* EdNodeB = FindEdGraphNodeForNode(NodeB);
	const FHeartPinGuid PinAGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(0));
	const FHeartPinGuid PinBGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(1));

	if (EdNodeA && EdNodeB)
	{
		auto&& EdGraphPinA = EdNodeA->FindPin(NodeA->GetPinDescChecked(PinAGuid).Name);
		auto&& EdGraphPinB = EdNodeB->FindPin(NodeB->GetPinDescChecked(PinBGuid).Name);

		if (EdGraphPinA && EdGraphPinB)
		{
			if (NodeA->GetLinks(PinAGuid, true).Links.Contains(NodeB->GetPinReference(PinBGuid)) &&
				NodeB->GetLinks(PinBGuid, true).Links.Contains(NodeA->GetPinReference(PinAGuid)))
			{
				EdGraphPinA->MakeLinkTo(EdGraphPinB);
			}
			else
			{
				EdGraphPinA->BreakLinkTo(EdGraphPinB);
			}
		}
	}
}