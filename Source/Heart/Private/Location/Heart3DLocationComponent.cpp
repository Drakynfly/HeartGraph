// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/Heart3DLocationComponent.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "StructUtils/InstancedStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Heart3DLocationComponent)

void UHeart3DLocationComponent::ExportMigrationData(FInstancedStruct& OutData)
{
	OutData.InitializeAs(FHeartNodeLocationMigrationData::StaticStruct());
	FHeartNodeLocationMigrationData& Data = OutData.GetMutable<FHeartNodeLocationMigrationData>();
	Data.Locations = Locations;
}

void UHeart3DLocationComponent::ImportMigrationData(const FInstancedStruct& InData)
{
	const FHeartNodeLocationMigrationData& Data = InData.Get<FHeartNodeLocationMigrationData>();
	Locations = Data.Locations;
}

FVector2D UHeart3DLocationComponent::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	const UHeartGraph* Graph = GetGraph();
	if (const FHeartNodeIndex Index = Graph->GetNodeIndex(Node);
		Locations.IsValidIndex(Index.Index))
	{
		return FVector2D(Locations[Index.Index]);
	}
	return FVector2D::ZeroVector;
}

void UHeart3DLocationComponent::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool)
{
	const UHeartGraph* Graph = GetGraph();
	const FHeartNodeIndex Index = Graph->GetNodeIndex(Node);
	if (Index.Index != INDEX_NONE)
	{
		if (!Locations.IsValidIndex(Index.Index))
		{
			Locations.SetNum(Index.Index + 1);
		}
		FVector& NodeLocation = Locations[Index.Index];
		NodeLocation.X = Location.X;
		NodeLocation.Y = Location.Y;
	}

	if (UHeartGraphNode* GraphNode = Graph->GetNode(Node))
	{
		GraphNode->OnNodeLocationChanged_Native.Broadcast(GraphNode->Guid, Location);
		GraphNode->OnNodeLocationChanged.Broadcast(GraphNode, Location);
	}
}

FVector UHeart3DLocationComponent::GetNodeLocation3D(const FHeartNodeGuid& Node) const
{
	const UHeartGraph* Graph = GetGraph();
	if (const FHeartNodeIndex Index = Graph->GetNodeIndex(Node);
		Locations.IsValidIndex(Index.Index))
	{
		return Locations[Index.Index];
	}
	return FVector::ZeroVector;
}

void UHeart3DLocationComponent::SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool)
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
		GraphNode->OnNodeLocationChanged_Native.Broadcast(GraphNode->Guid, FVector2D(Location));
		GraphNode->OnNodeLocationChanged.Broadcast(GraphNode, FVector2D(Location));
	}
}
