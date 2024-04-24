// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "GraphRegistry/HeartNodeSource.h" // @todo move FHeartNodeArchetype to its own header?

class UHeartGraph;
class IHeartGraphInterface;

namespace Heart::API
{
	class FNodeCreator
	{
	public:
		// Create a HeartGraphNode that is the outer of its own instanced NodeObject, created from the NodeObjectClass.
		static UHeartGraphNode* CreateNode_Instanced(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
			const UClass* NodeObjectClass, const FVector2D& Location, UObject* NodeSpawningContext = nullptr);

		// Create a HeartGraphNode whose NodeObject is a reference to an external object.
		static UHeartGraphNode* CreateNode_Reference(UHeartGraph* Graph, const TSubclassOf<UHeartGraphNode>& GraphNodeClass,
			const UObject* NodeObject, const FVector2D& Location, UObject* NodeSpawningContext = nullptr);
	};

	/*
	 * Central API for creating and deleting nodes in a HeartGraph. All creation and deletion calls are pooled and then
	 * run in the dtor. This allows us to batch connection rewiring, and broadcast fewer events in a single frame.
	 * Alternatively, FNodeEdit may be kept alive over several frames, and accumulate multiple edits to make in one shot.
	 */
	class FNodeEdit
	{
	public:
		FNodeEdit(IHeartGraphInterface* GraphInterface);

		// Dtor runs any remaining pending edits.
		~FNodeEdit();

		using FNewNodeId = int32;

		/**
		 * Queues a node to be created
		 * @param Archetype The class and NodeSource used
		 * @param Location Initial location to spawn the node at
		 * @param NodeSpawningContext This is an optional UObject to finalize node creation. It's up to the subclass of
		 * heart node to use this. The object is *not* kept alive here, it's up to calling code to keep the context alive
		 * if this edit persists through GC.
		 */
		FNewNodeId Create(const FHeartNodeArchetype& Archetype, const FVector2D& Location, UObject* NodeSpawningContext = nullptr);

		// Create a HeartGraphNode that is the outer of its own instanced NodeObject, created from the NodeObjectClass.
		FNewNodeId Create_Instanced(TSubclassOf<UHeartGraphNode> GraphNodeClass, const UClass* NodeObjectClass, const FVector2D& Location, UObject* NodeSpawningContext = nullptr);

		// Create a HeartGraphNode whose NodeObject is a reference to an external object.
		FNewNodeId Create_Reference(TSubclassOf<UHeartGraphNode> GraphNodeClass, const UObject* NodeObject, const FVector2D& Location, UObject* NodeSpawningContext = nullptr);

		// Create from template graph class and node object
		template <
			typename THeartGraphNode
			UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value)
		>
		FNewNodeId CreateFromObject(UObject* NodeObject, const FVector2D& Location, UObject* NodeSpawningContext = nullptr)
		{
			checkf(!NodeObject->IsA<UHeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node object instead of an 'OBJECT' node class"));
			return Create_Reference(THeartGraphNode::StaticClass(), NodeObject, Location, NodeSpawningContext);
		}

		// Create from template node class and attempt to cast the return to the template graph class
		template <
			typename THeartGraphNode,
			typename TNodeObject
			UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value &&
						!TIsDerivedFrom<TNodeObject, UHeartGraphNode>::Value)
		>
		FNewNodeId CreateFromClass(const FVector2D& Location, UObject* NodeSpawningContext = nullptr)
		{
			return Create_Instanced(THeartGraphNode::StaticClass(), TNodeObject::StaticClass(), Location, NodeSpawningContext);
		}

		// Create from node class and attempt to cast the return to the template graph class
		template <
			typename THeartGraphNode
			UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value)
		>
		FNewNodeId CreateFromClass(const TSubclassOf<UObject> NodeClass, const FVector2D& Location, UObject* NodeSpawningContext = nullptr)
		{
			checkf(!NodeClass->IsChildOf<THeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node class instead of an 'OBJECT' node class"));
			return Create_Instanced(THeartGraphNode::StaticClass(), NodeClass, Location, NodeSpawningContext);
		}

		// Retrieve the GraphNode for a pending creation.
		// This node will *not* have a HeartGraph yet; GetOwningGraph/GetGraph/GetGraphTyped will all fail
		[[nodiscard]] UHeartGraphNode* GetGraphNode(FNewNodeId Id) const;

		/**
		 * Queues a node for deletion.
		 */
		void Delete(const FHeartNodeGuid& NodeGuid);

		/**
		 * Handling all pending creation and deleting requests now. This normally does not need to be called, unless
		 * re-using a FNodeEdit, and the caller expects some actions to be complete before queuing others.
		 */
		void RunNow();

	private:
		void HandlePending();

		// The graph we are editing
		TObjectPtr<UHeartGraph> Graph;

		struct FPendingCreate
		{
			TObjectPtr<UHeartGraphNode> Node;
		};

		struct FPendingDelete
		{
			FHeartNodeGuid Guid;
		};

		TArray<FPendingCreate> PendingCreates;
		TArray<FPendingDelete> PendingDeletes;
	};
}