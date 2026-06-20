// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeEdit.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

namespace Heart::API
{
	UHeartGraphNode* FNodeCreator::CreateNode_Instanced(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UClass* NodeObjectClass, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeObjectClass));

		UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = NewObject<UObject>(NewGraphNode, NodeObjectClass);

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	UHeartGraphNode* FNodeCreator::CreateNode_Duplicate(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UObject* NodeTemplate, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeTemplate));

		UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = DuplicateObject(NodeTemplate, Graph);

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	UHeartGraphNode* FNodeCreator::CreateNode_Reference(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UObject* NodeObject, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeObject));

		auto&& NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = const_cast<UObject*>(NodeObject); // @todo temp const_cast in lieu of proper const safety enforcement

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	FNodeEdit::~FNodeEdit()
	{
		HandlePending();
	}

	bool FNodeEdit::AddNode(TNotNull<UHeartGraph*> Graph, UHeartGraphNode* Node)
	{
		checkSlow(Node->GetOuter() == Cast<UObject>(&Graph));

		const FHeartNodeGuid& NodeGuid = Node->GetGuid();

		if (!ensure(IsValid(Node) && NodeGuid.IsValid()))
		{
			UE_LOG(LogHeartGraph, Error, TEXT("Tried to add invalid node!"))
			return false;
		}

		// @todo uncomment this and if something is tripping it, solve that!
		/*
		if (!ensure(IsValid(Node->GetNodeObject())))
		{
			UE_LOG(LogHeartGraph, Error, TEXT("Tried to add a node with invalid object!"))
			return;
		}
		*/

		if (!ensure(!Graph->Nodes.Contains(NodeGuid)))
		{
			UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
			return false;
		}

		Graph->Nodes.Add(NodeGuid, Node);
		for (auto&& Element : Node->GetDefaultComponents())
		{
			Graph->NodeComponents.FindOrAdd(Element->GetClass()).Components.Add(NodeGuid, DuplicateObject(Element, Graph));
		}
		Node->OnAddedToGraph(Graph, NodeGuid);
		FHeartNodeAddOrRemoveEvent Event;
		Event.Type = EHeartNodeAddOrRemoveEventType::Add;
		Event.Nodes.Add(NodeGuid);
		Graph->HandleNodeAddOrRemoveEvent(Event);
		return true;
	}

	bool FNodeEdit::DeleteNode(const TNotNull<UHeartGraph*> Graph, const FHeartNodeGuid& Node)
	{
		if (!ensure(Node.IsValid()))
		{
			return false;
		}

		if (!Graph->Nodes.Contains(Node))
		{
			return false;
		}

		Graph->RemoveComponentsForNode(Node);

		FPinEdit(Graph).DisconnectAll(Node);

		TObjectPtr<UHeartGraphNode> NodeBeingRemoved = nullptr;
		Graph->Nodes.RemoveAndCopyValue(Node, NodeBeingRemoved);
		if (IsValid(NodeBeingRemoved))
		{
			NodeBeingRemoved->OnRemovedFromGraph(Graph, Node);

			FHeartNodeAddOrRemoveEvent Event;
			Event.Type = EHeartNodeAddOrRemoveEventType::Remove;
			Event.Nodes.Add(Node);
			Graph->HandleNodeAddOrRemoveEvent(Event);
			return true;
		}

		return true;
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create(const FHeartNodeArchetype& Archetype, const FVector2D& Location,
											UObject* NodeSpawningContext)
	{
		UHeartGraphNode* NewGraphNode;

		// @todo this would not work, in the rare edge case of making a node with a UClass as a referenced source
		if (const UClass* AsClass = Archetype.Source.As<UClass>())
		{
			NewGraphNode = FNodeCreator::CreateNode_Instanced(Graph, Archetype.GraphNode, AsClass, NodeSpawningContext);
		}
		else
		{
			NewGraphNode = FNodeCreator::CreateNode_Reference(Graph, Archetype.GraphNode, Archetype.Source.As<UObject>(), NodeSpawningContext);
		}
		checkSlow(NewGraphNode)

		return PendingCreates.Add({NewGraphNode, Location});
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Instanced(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
													  const UClass* NodeObjectClass, const FVector2D& Location,
													  UObject* NodeSpawningContext)
	{
		return PendingCreates.Add({FNodeCreator::CreateNode_Instanced(Graph, GraphNodeClass, NodeObjectClass, NodeSpawningContext), Location});
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Duplicate(const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
		const UObject* NodeTemplate, const FVector2D& Location, UObject* NodeSpawningContext)
	{
		return PendingCreates.Add({FNodeCreator::CreateNode_Duplicate(Graph, GraphNodeClass, NodeTemplate, NodeSpawningContext), Location});
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Reference(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
													  const UObject* NodeObject, const FVector2D& Location,
													  UObject* NodeSpawningContext)
	{
		return PendingCreates.Add({FNodeCreator::CreateNode_Reference(Graph, GraphNodeClass, NodeObject, NodeSpawningContext), Location});
	}

	UHeartGraphNode* FNodeEdit::GetGraphNode(const FNewNodeId Id) const
	{
		return PendingCreates[Id].Node;
	}

	UHeartGraphNode* FNodeEdit::Get() const
	{
		return PendingCreates.Last().Node;
	}

	void FNodeEdit::Delete(const FHeartNodeGuid& NodeGuid)
	{
		PendingDeletes.AddUnique(NodeGuid);
	}

	void FNodeEdit::RunNow()
	{
		HandlePending();
		PendingCreates.Empty();
		PendingDeletes.Empty();
	}

	void FNodeEdit::HandlePending()
	{
		if (!PendingDeletes.IsEmpty())
		{
			// Pending delete pass 0: Verify and clean
			for (auto&& It = PendingDeletes.CreateIterator(); It; ++It)
			{
				// Remove any guids that aren't valid for some reason
				if (!ensure(It->IsValid()) || !Graph->Nodes.Contains(*It))
				{
					It.RemoveCurrent();
				}
			}

			// Pending delete pass 1: Remove node components
			Graph->RemoveComponentsForNodes(PendingDeletes);

			// Pending delete pass 2: Remove all connections
			{
				FPinEdit ConnectionsEdit(Graph);
				for (auto&& PendingDelete : PendingDeletes)
				{
					// Remove all connections that will be orphaned by removing this node
					ConnectionsEdit.DisconnectAll(PendingDelete);
				}
				// Out-of-scope for ConnectionsEdit, connections changed event is broadcast
			}

			// Pending delete pass 3: Remove the nodes
			{
				FHeartNodeAddOrRemoveEvent Event;
				Event.Type = EHeartNodeAddOrRemoveEventType::Remove;
				Event.Nodes = PendingDeletes;

				for (auto&& PendingDelete : PendingDeletes)
				{
					TObjectPtr<UHeartGraphNode> NodeBeingRemoved;
					Graph->Nodes.RemoveAndCopyValue(PendingDelete, NodeBeingRemoved);
					if (IsValid(NodeBeingRemoved))
					{
						NodeBeingRemoved->OnRemovedFromGraph(Graph, PendingDelete);
					}
				}

				Graph->HandleNodeAddOrRemoveEvent(Event);
			}
		}

		if (!PendingCreates.IsEmpty())
		{
			FHeartNodeAddOrRemoveEvent Event;
			Event.Type = EHeartNodeAddOrRemoveEventType::Add;

			IHeartNodeLocationInterface* LocationInterface = Graph->GetNodeLocationInterface();

			// Pending create pass
			for (auto&& Pending : PendingCreates)
			{
				if (!ensure(IsValid(Pending.Node->GetNodeObject())))
				{
					UE_LOG(LogHeartGraph, Error, TEXT("Tried to add a node with invalid object!"))
					continue;
				}

				const FHeartNodeGuid& NodeGuid = Pending.Node->GetGuid();

				if (!ensure(!Graph->Nodes.Contains(NodeGuid)))
				{
					UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
					continue;
				}

				Graph->Nodes.Add(NodeGuid, Pending.Node);

				LocationInterface->SetNodeLocation(NodeGuid, Pending.Location, false);

				for (auto&& Element : Pending.Node->GetDefaultComponents())
				{
					Graph->NodeComponents.FindOrAdd(Element->GetClass()).Components.Add(NodeGuid, DuplicateObject(Element, Graph));
				}

				Pending.Node->OnAddedToGraph(Graph, NodeGuid);
				Event.Nodes.Add(NodeGuid);
			}

			Graph->HandleNodeAddOrRemoveEvent(Event);
		}
	}
}