// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartLayoutHelper.h"
#include "ModelView/HeartNodeLocationAccessor.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayoutHelper)

bool UHeartLayoutHelper::Layout(IHeartNodeLocationAccessor* Accessor)
{
	const UHeartGraph* Graph = Accessor->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<FHeartNodeGuid> AllNodes;
		Graph->GetNodeGuids(AllNodes);
		return Layout(Accessor, AllNodes);
	}

	return false;
}

bool UHeartLayoutHelper::Layout(IHeartNodeLocationAccessor* Accessor, const float DeltaTime)
{
	const UHeartGraph* Graph = Accessor->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<FHeartNodeGuid> AllNodes;
		Graph->GetNodeGuids(AllNodes);
		return Layout(Accessor, AllNodes, DeltaTime);
	}

	return false;
}

FHeartGraphAdjacencyList UHeartLayoutHelper::GetGraphAdjacencyList(const IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes) const
{
	FHeartGraphAdjacencyList Result;

	auto Graph = Accessor->GetHeartGraph();

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		auto&& NodeGuid = Nodes[i];
		auto&& Node = Graph->GetNode(NodeGuid);

		TArray<int32>& NodeAdjacency = Result.AdjacencyList.AddDefaulted_GetRef();
		Node->FindPinsByDirection(EHeartPinDirection::Output)
			.ForEach([Node, &NodeAdjacency, &Nodes](const FHeartPinGuid Pin)
			{
				auto&& Connections = Node->GetConnections(Pin);
				if (!Connections.IsSet())
				{
					return;
				}

				for (auto&& Connection : Connections.GetValue())
				{
					if (const int32 NodeIndex = Nodes.Find(Connection.NodeGuid);
						NodeIndex != INDEX_NONE)
					{
						NodeAdjacency.Add(NodeIndex);
					}
				}
			});
	}

	return Result;
}

void UHeartLayoutHelper::ApplyNewPositions(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor,
										   const TArray<FHeartNodeGuid>& Nodes,
										   const TArray<FVector2D>& NewPositions) const
{
    for (int32 i = 0; i < NewPositions.Num(); ++i)
    {
    	Accessor->SetNodeLocation(Nodes[i], NewPositions[i], false);
    }

	// @todo fix
	static constexpr bool TempInProgressToPreventNetReplication = true;

	TSet<FHeartNodeGuid> Touched;
	Touched.Append(Nodes);
    Accessor->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, TempInProgressToPreventNetReplication);
}

bool UHeartLayoutHelper_BlueprintBase::Layout(IHeartNodeLocationAccessor* Accessor,
											  const TArray<FHeartNodeGuid>& Nodes)
{
	return Layout_BP(Accessor->_getUObject(), Nodes);
}