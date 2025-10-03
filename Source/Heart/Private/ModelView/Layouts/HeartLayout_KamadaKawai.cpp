// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_KamadaKawai.h"
#include "Model/HeartGraphInterface.h"
#include "Algorithms/Nodesoup.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayout_KamadaKawai)

bool UHeartLayout_KamadaKawai::Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes)
{
	const FHeartGraphAdjacencyList GraphAdjacencyList = GetGraphAdjacencyList(Interface, Nodes);

	const TArray<FVector2D> NewPositions = Nodesoup::kamada_kawai(GraphAdjacencyList.AdjacencyList, Width, Height, Strength, EnergyThreshold);

	ApplyNewPositions(Cast<UObject>(Interface), Nodes, NewPositions);

	return true;
}