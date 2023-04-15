// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_KamadaKawai.h"

#include "Algorithms/Nodesoup.h"
#include "Model/HeartGraphNode.h"

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

		TArray<UHeartGraphPin*> Pins = Node->GetOutputPins();

		for (auto&& Pin : Pins)
		{
			TArray<UHeartGraphPin*> Connections = Pin->GetAllConnections();

			for (auto&& Connection : Connections)
			{
				TEMPARRAY.Add(Nodes.Find(Connection->GetNode()));
			}
		}

		GraphAdjacencyList.Add(TEMPARRAY);
	}

	TArray<FVector2D> NewPositions = Nodesoup::kamada_kawai(GraphAdjacencyList, Width, Height, Strength, EnergyThreshold);

	for (int32 i = 0; i < NewPositions.Num(); ++i)
	{
		Nodes[i]->SetLocation(NewPositions[i]);
	}

	return true;
}
