// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphInterface)

FVector2D IHeartGraphInterface::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	if (auto&& GraphNode = GetHeartGraph()->GetNode(Node))
	{
		return GraphNode->GetLocation();
	}
	return FVector2D::ZeroVector;
}

void IHeartGraphInterface::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove)
{
	if (auto&& GraphNode = GetHeartGraph()->GetNode(Node))
	{
		GraphNode->SetLocation(Location);
	}
}

FVector IHeartGraphInterface3D::GetNodeLocation3D(const FHeartNodeGuid& Node) const
{
	if (auto&& GraphNode = GetHeartGraph()->GetNode(Node))
	{
		if (auto&& GraphNode3D = Cast<UHeartGraphNode3D>(GraphNode))
		{
			return GraphNode3D->GetLocation3D();
		}
		return FVector(GraphNode->GetLocation(), 0.0);
	}
	return FVector::ZeroVector;
}

void IHeartGraphInterface3D::SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove)
{
	if (auto&& GraphNode = GetHeartGraph()->GetNode(Node))
	{
		if (auto&& GraphNode3D = Cast<UHeartGraphNode3D>(GraphNode))
		{
			GraphNode3D->SetLocation3D(Location);
		}
		else
		{
			GraphNode->SetLocation(FVector2D(Location));
		}
	}
}

FVector2D UHeartNodeLocationAccessorLibrary::GetNodeLocation_Pointer(
	const TScriptInterface<IHeartGraphInterface>& Accessor, UHeartGraphNode* Node)
{
	return Accessor->GetNodeLocation(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation_Pointer(
	const TScriptInterface<IHeartGraphInterface>& Accessor, UHeartGraphNode* Node, const FVector2D& Location, const bool InProgressMove)
{
	Accessor->SetNodeLocation(Node->GetGuid(), Location, InProgressMove);
}

FVector UHeartNodeLocationAccessorLibrary::GetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode3D* Node)
{
	return Accessor->GetNodeLocation3D(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode3D* Node, const FVector& Location, const bool InProgressMove)
{
	Accessor->SetNodeLocation3D(Node->GetGuid(), Location, InProgressMove);
}