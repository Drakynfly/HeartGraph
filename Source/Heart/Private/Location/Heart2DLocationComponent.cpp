// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/Heart2DLocationComponent.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "StructUtils/InstancedStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Heart2DLocationComponent)

void UHeart2DLocationComponent::ExportMigrationData(FInstancedStruct& OutData)
{
	OutData.InitializeAs(FHeartNodeLocationMigrationData::StaticStruct());
	FHeartNodeLocationMigrationData& Data = OutData.GetMutable<FHeartNodeLocationMigrationData>();
	Data.Locations.Reserve(Locations.Num());
	for (auto&& Element : Locations)
	{
		Data.Locations.Add(FVector(Element, 0.0));
	}
}

void UHeart2DLocationComponent::ImportMigrationData(const FInstancedStruct& InData)
{
	const FHeartNodeLocationMigrationData& Data = InData.Get<FHeartNodeLocationMigrationData>();
	Locations.Reset(Data.Locations.Num());
	for (auto&& Element : Data.Locations)
	{
		Locations.Add(FVector2D(Element));
	}
}

FVector2D UHeart2DLocationComponent::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	const UHeartGraph* Graph = GetGraph();
	if (const FHeartNodeIndex Index = Graph->GetNodeIndex(Node);
		Locations.IsValidIndex(Index.Index))
	{
		return Locations[Index.Index];
	}
	return FVector2D::ZeroVector;
}

void UHeart2DLocationComponent::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool)
{
	const UHeartGraph* Graph = GetGraph();
	const FHeartNodeIndex Index = Graph->GetNodeIndex(Node);
	if (Index.Index != INDEX_NONE)
	{
		if (!Locations.IsValidIndex(Index.Index))
		{
			Locations.SetNum(Index.Index + 1);
		}
		Locations[Index.Index] = Location;
	}

	if (UHeartGraphNode* GraphNode = Graph->GetNode(Node))
	{
		GraphNode->OnNodeLocationChanged_Native.Broadcast(GraphNode->GetGuid(), Location);
		GraphNode->OnNodeLocationChanged.Broadcast(GraphNode, Location);
	}
}
