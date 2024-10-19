// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_KamadaKawai.h"
#include "ModelView/HeartNodeLocationAccessor.h"
#include "Algorithms/Nodesoup.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayout_KamadaKawai)

bool UHeartLayout_KamadaKawai::Layout(IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes)
{
	const FHeartGraphAdjacencyList GraphAdjacencyList = GetGraphAdjacencyList(Accessor, Nodes);

	const TArray<FVector2D> NewPositions = Nodesoup::kamada_kawai(GraphAdjacencyList.AdjacencyList, Width, Height, Strength, EnergyThreshold);

	ApplyNewPositions(Accessor->_getUObject(), Nodes, NewPositions);

	return true;
}