// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeEdit.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphNode.h"

namespace Heart::API
{
	UHeartGraphNode* FNodeCreator::CreateNode_Instanced(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UClass* NodeObjectClass, const FVector2D& Location, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeObjectClass));

		UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = NewObject<UObject>(NewGraphNode, NodeObjectClass);
		NewGraphNode->Location = Location;

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	UHeartGraphNode* FNodeCreator::CreateNode_Duplicate(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UObject* NodeTemplate, const FVector2D& Location, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeTemplate));

		UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = DuplicateObject(NodeTemplate, Graph);
		NewGraphNode->Location = Location;

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	UHeartGraphNode* FNodeCreator::CreateNode_Reference(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
														const UObject* NodeObject, const FVector2D& Location, UObject* NodeSpawningContext)
	{
		checkSlow(IsValid(GraphNodeClass));
		checkSlow(IsValid(NodeObject));

		auto&& NewGraphNode = NewObject<UHeartGraphNode>(Graph, GraphNodeClass);
		NewGraphNode->Guid = FHeartNodeGuid::New();
		NewGraphNode->NodeObject = const_cast<UObject*>(NodeObject); // @todo temp const_cast in lieu of proper const safety enforcement
		NewGraphNode->Location = Location;

		NewGraphNode->OnCreate(NodeSpawningContext);

		return NewGraphNode;
	}

	FNodeEdit::FNodeEdit(IHeartGraphInterface* GraphInterface)
	{
		if (ensureAlways(GraphInterface))
		{
			Graph = GraphInterface->GetHeartGraph();
			checkSlow(IsValid(Graph))
		}
	}

	FNodeEdit::~FNodeEdit()
	{
		HandlePending();
	}

	bool FNodeEdit::DeleteNode(IHeartGraphInterface* GraphInterface, const FHeartNodeGuid& Node)
	{
		UHeartGraph* GraphPtr = GraphInterface->GetHeartGraph();
		checkSlow(IsValid(GraphPtr))

		if (!ensure(Node.IsValid()))
		{
			return false;
		}

		if (!GraphPtr->Nodes.Contains(Node))
		{
			return false;
		}

		GraphPtr->RemoveComponentsForNode(Node);

		FPinEdit(GraphPtr).DisconnectAll(Node);

		UHeartGraphNode* NodeBeingRemoved = nullptr;
		GraphPtr->Nodes.RemoveAndCopyValue(Node, ObjectPtrWrap(NodeBeingRemoved));
		if (IsValid(NodeBeingRemoved))
		{
			FHeartNodeRemoveEvent Event;
			Event.AffectedNodes.Add(NodeBeingRemoved);
			GraphPtr->HandleNodeRemoveEvent(Event);
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
			NewGraphNode = FNodeCreator::CreateNode_Instanced(Graph, Archetype.GraphNode, AsClass, Location, NodeSpawningContext);
		}
		else
		{
			NewGraphNode = FNodeCreator::CreateNode_Reference(Graph, Archetype.GraphNode, Archetype.Source.As<UObject>(), Location, NodeSpawningContext);
		}
		checkSlow(NewGraphNode)

		return PendingCreates.Emplace(NewGraphNode);
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Instanced(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
													  const UClass* NodeObjectClass, const FVector2D& Location,
													  UObject* NodeSpawningContext)
	{
		return PendingCreates.Emplace(FNodeCreator::CreateNode_Instanced(Graph, GraphNodeClass, NodeObjectClass, Location, NodeSpawningContext));
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Duplicate(const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
		const UObject* NodeTemplate, const FVector2D& Location, UObject* NodeSpawningContext)
	{
		return PendingCreates.Emplace(FNodeCreator::CreateNode_Duplicate(Graph, GraphNodeClass, NodeTemplate, Location, NodeSpawningContext));
	}

	FNodeEdit::FNewNodeId FNodeEdit::Create_Reference(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
													  const UObject* NodeObject, const FVector2D& Location,
													  UObject* NodeSpawningContext)
	{
		return PendingCreates.Emplace(FNodeCreator::CreateNode_Reference(Graph, GraphNodeClass, NodeObject, Location, NodeSpawningContext));
	}

	UHeartGraphNode* FNodeEdit::GetGraphNode(const FNewNodeId Id) const
	{
		return PendingCreates[Id];
	}

	UHeartGraphNode* FNodeEdit::Get() const
	{
		return PendingCreates.Last();
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
				FHeartNodeRemoveEvent Event;

				for (auto&& PendingDelete : PendingDeletes)
				{
					UHeartGraphNode* NodeBeingRemoved;
					Graph->Nodes.RemoveAndCopyValue(PendingDelete, ObjectPtrWrap(NodeBeingRemoved));
					if (IsValid(NodeBeingRemoved))
					{
						Event.AffectedNodes.Add(NodeBeingRemoved);
					}
				}

				Graph->HandleNodeRemoveEvent(Event);
			}
		}

		if (!PendingCreates.IsEmpty())
		{
			FHeartNodeAddEvent Event;

			// Pending create pass
			for (auto&& Element : PendingCreates)
			{
				if (!ensure(IsValid(Element)))
				{
					UE_LOG(LogHeartGraph, Error, TEXT("Tried to add invalid node!"))
					continue;
				}

				if (!ensure(IsValid(Element->GetNodeObject())))
				{
					UE_LOG(LogHeartGraph, Error, TEXT("Tried to add a node with invalid object!"))
					continue;
				}

				const FHeartNodeGuid& NodeGuid = Element->GetGuid();

				if (!ensure(!Graph->Nodes.Contains(NodeGuid)))
				{
					UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
					continue;
				}

				Graph->Nodes.Add(NodeGuid, Element);
				Event.NewNodes.Add(NodeGuid);
			}

			Graph->HandleNodeAddEvent(Event);
		}
	}
}