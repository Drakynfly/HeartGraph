// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"

#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

DEFINE_LOG_CATEGORY(LogHeartGraph)

UWorld* UHeartGraph::GetWorld() const
{
	if (!IsTemplate())
	{
		UWorld* World = nullptr;
		if (GetSchema()->TryGetWorldForGraph(this, World))
		{
			return World;
		}
	}

	return Super::GetWorld();
}

void UHeartGraph::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

#if WITH_EDITOR
	// The HeartEdGraph doesn't need to persist for graphs duplicated during gameplay
	if (GetWorld()->IsGameWorld())
	{
		HeartEdGraph = nullptr;
	}
#endif
}

void UHeartGraph::NotifyNodeConnectionsChanged(const TArray<UHeartGraphNode*>& AffectedNodes, const TArray<FHeartPinGuid>& AffectedPins)
{
	FHeartGraphConnectionEvent Event;
	Event.AffectedNodes = AffectedNodes;
	Event.AffectedPins = AffectedPins;
	NotifyNodeConnectionsChanged(Event);
}

void UHeartGraph::NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event)
{
	BP_OnNodeConnectionsChanged(Event);
	OnNodeConnectionsChanged.Broadcast(Event);
}

#if WITH_EDITOR
void UHeartGraph::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Save the EdGraph with us in the editor
	UHeartGraph* This = CastChecked<UHeartGraph>(InThis);
	Collector.AddReferencedObject(This->HeartEdGraph, This);

	Super::AddReferencedObjects(InThis, Collector);
}
#endif

UHeartGraph* UHeartGraph::GetHeartGraph_Implementation() const
{
	return const_cast<UHeartGraph*>(this);
}

void UHeartGraph::ForEachNode(const TFunctionRef<bool(UHeartGraphNode*)>& Iter) const
{
	for (auto&& Element : Nodes)
	{
		if (ensure(Element.Value))
		{
			if (Iter(Element.Value)) return;
		}
	}
}

UHeartGraphNode* UHeartGraph::GetNode(const FHeartNodeGuid& NodeGuid) const
{
	auto&& Result = Nodes.Find(NodeGuid);
	return Result ? *Result : nullptr;
}

TSubclassOf<UHeartGraphSchema> UHeartGraph::GetSchemaClass_Implementation() const
{
	return UHeartGraphSchema::StaticClass();
}

const UHeartGraphSchema* UHeartGraph::GetSchema() const
{
	// Always return the schema for the CDO
	return GetSchemaStatic<UHeartGraphSchema>(GetClass());
}

const UHeartGraphSchema* UHeartGraph::GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const
{
	return GetSchema();
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeObject(UObject* NodeObject, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& NodeRegistry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		GraphNodeClass = NodeRegistry->GetRegistry(GetClass())->GetGraphNodeClassForNode(NodeObject->GetClass());
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeObject->GetClass()->GetName())
		return nullptr;
	}

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::NewGuid();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NodeObject;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeClass(const UClass* NodeClass, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& NodeRegistry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		GraphNodeClass = NodeRegistry->GetRegistry(GetClass())->GetGraphNodeClassForNode(NodeClass);
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeClass->GetName())
		return nullptr;
	}

	// The graph has to be the outer for the NodeObjects or Unreal will kill them when recompiling the heart graph blueprint
	// This is probably just a issue with the current version of unreal (5.1.0), but even if it's fixed, it's probably fine
	// to leave it like this.
	auto&& NewNodeObject = NewObject<UObject>(this, NodeClass);

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::NewGuid();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NewNodeObject;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNodeFromClass(const UClass* NodeClass, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeClass)))
	{
		return nullptr;
	}

	return CreateNodeForNodeClass(NodeClass, Location);
}

UHeartGraphNode* UHeartGraph::CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeObject)))
	{
		return nullptr;
	}

	return CreateNodeForNodeObject(NodeObject, Location);
}

void UHeartGraph::AddNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add invalid node!"))
		return;
	}

	if (!ensure(IsValid(Node->GetNodeObject())))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node with invalid object!"))
		return;
	}

	const FHeartNodeGuid NodeGuid = Node->GetGuid();

	if (Nodes.Contains(NodeGuid))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
		return;
	}

	Nodes.Add(NodeGuid, Node);

#if WITH_EDITOR
	if (HeartEdGraph)
	{
		if (auto&& EdGraphNode = Node->GetEdGraphNode())
		{
			HeartEdGraph->AddNode(EdGraphNode);
		}
		else
		{
			OnNodeCreatedInEditorExternally.ExecuteIfBound(Node);
		}
	}
#endif

	OnNodeAdded.Broadcast(Node);
}

bool UHeartGraph::RemoveNode(const FHeartNodeGuid& NodeGuid)
{
	if (!ensure(NodeGuid.IsValid()))
	{
		return false;
	}

	auto&& NodeBeingRemoved = Nodes.Find(NodeGuid);
	auto&& Removed = Nodes.Remove(NodeGuid);

#if WITH_EDITOR
	if (HeartEdGraph)
	{
		if (auto&& EdGraphNode = (*NodeBeingRemoved)->GetEdGraphNode())
		{
			HeartEdGraph->RemoveNode(EdGraphNode);
		}
	}
#endif

	if (NodeBeingRemoved)
	{
		OnNodeRemoved.Broadcast(*NodeBeingRemoved);
	}

	return Removed > 0;
}

bool UHeartGraph::ConnectPins(const FHeartGraphPinReference PinA, const FHeartGraphPinReference PinB)
{
	UHeartGraphNode* ANode = GetNode(PinA.NodeGuid);
	UHeartGraphNode* BNode = GetNode(PinB.NodeGuid);

	if (!ensure(IsValid(ANode) && IsValid(BNode)))
	{
		return false;
	}

	// Add to both lists
	ANode->GetLinks(PinA.PinGuid).Links.Add(PinB);
	BNode->GetLinks(PinB.PinGuid).Links.Add(PinA);

#if WITH_EDITOR
	if (ANode->GetEdGraphNode() && BNode->GetEdGraphNode())
	{
		auto&& ThisEdGraphPin = ANode->GetEdGraphNode()->FindPin(ANode->GetPinDesc(PinA.PinGuid).Name);
		auto&& OtherEdGraphPin = BNode->GetEdGraphNode()->FindPin(BNode->GetPinDesc(PinB.PinGuid).Name);

		if (ThisEdGraphPin && OtherEdGraphPin)
		{
			ThisEdGraphPin->MakeLinkTo(OtherEdGraphPin);
		}
	}
#endif

	return true;
}

bool UHeartGraph::DisconnectPins(const FHeartGraphPinReference PinA, const FHeartGraphPinReference PinB)
{
	UHeartGraphNode* ANode = GetNode(PinA.NodeGuid);
	UHeartGraphNode* BNode = GetNode(PinB.NodeGuid);

	if (!ensure(IsValid(ANode) && IsValid(BNode)))
	{
		return false;
	}

	// We assume that both of these are true, but proceed anyway if only one of them are...
	if (ANode->GetLinks(PinA.NodeGuid).Links.Contains(PinB) ||
		BNode->GetLinks(PinB.NodeGuid).Links.Contains(PinA))
	{
		ANode->GetLinks(PinA.NodeGuid).Links.Remove(PinB);
		BNode->GetLinks(PinB.NodeGuid).Links.Remove(PinA);

#if WITH_EDITOR
		if (ANode->GetEdGraphNode() && BNode->GetEdGraphNode())
		{
			auto&& ThisEdGraphPin = ANode->GetEdGraphNode()->FindPin(ANode->GetPinDesc(PinA.PinGuid).Name);
			auto&& OtherEdGraphPin = BNode->GetEdGraphNode()->FindPin(BNode->GetPinDesc(PinB.PinGuid).Name);

			if (ThisEdGraphPin && OtherEdGraphPin)
			{
				ThisEdGraphPin->BreakLinkTo(OtherEdGraphPin);
			}
		}
#endif

		ANode->NotifyPinConnectionsChanged(PinA.PinGuid);
		BNode->NotifyPinConnectionsChanged(PinB.PinGuid);
		NotifyNodeConnectionsChanged({ANode, BNode}, {PinA.PinGuid, PinB.PinGuid});
	}

	return true;
}

void UHeartGraph::DisconnectAllPins(const FHeartGraphPinReference Pin)
{
	UHeartGraphNode* ANode = GetNode(Pin.NodeGuid);

	if (!ensure(IsValid(ANode)))
	{
		return;
	}

	for (const FHeartGraphPinReference& Link : ANode->GetLinks(Pin.PinGuid).Links)
	{
		DisconnectPins(Pin, Link);
	}
}
