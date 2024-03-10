// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_KamadaKawai.h"

#include "Algorithms/Nodesoup.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartNodeLocationAccessor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayout_KamadaKawai)

bool UHeartLayout_KamadaKawai::Layout(IHeartNodeLocationAccessor* Accessor, const TArray<UHeartGraphNode*>& Nodes) const
{
	// @TODO THIS IS ALL KINDA AWFUL. REFACTOR AT EARLIEST CONVENIENCE
	// - Add profiling
	// - Allow for passing in a precached adjacency list

	TArray<TArray<int32>> TEMPGRAPHARRAYS;

	TArray<TArrayView<const int32>> GraphAdjacencyList;

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		const UHeartGraphNode* Node = Nodes[i];

		TArray<int32>& TEMPARRAY = TEMPGRAPHARRAYS.AddDefaulted_GetRef();

		TArray<FHeartPinGuid> Pins = Node->GetOutputPins();

		for (auto&& Pin : Pins)
		{
			auto&& Connections = Node->GetConnections(Pin);
			if (!Connections.IsSet())
			{
				continue;
			}

			for (auto&& Connection : Connections.GetValue().Links)
			{
				TEMPARRAY.Add(Nodes.Find(Node->GetGraph()->GetNode(Connection.NodeGuid)));
			}
		}

		GraphAdjacencyList.Add(TEMPARRAY);
	}

	TArray<FVector2D> NewPositions = Nodesoup::kamada_kawai(GraphAdjacencyList, Width, Height, Strength, EnergyThreshold);

	TSet<UHeartGraphNode*> Touched;

	for (int32 i = 0; i < NewPositions.Num(); ++i)
	{
		auto Node = Nodes[i];
		Touched.Add(Node);

		Accessor->SetNodeLocation(Node->GetGuid(), NewPositions[i], false);
	}

	Accessor->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, false);

	return true;
}