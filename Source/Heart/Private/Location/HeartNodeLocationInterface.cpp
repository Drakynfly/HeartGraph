// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/HeartNodeLocationInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

FVector2D UHeartNodeLocationAccessorLibrary::GetNodeLocation_Pointer(
	const TScriptInterface<IHeartNodeLocationInterface>& Accessor, UHeartGraphNode* Node)
{
	return Accessor->GetNodeLocation(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation_Pointer(
	const TScriptInterface<IHeartNodeLocationInterface>& Accessor, UHeartGraphNode* Node, const FVector2D& Location, const bool InProgressMove)
{
	Accessor->SetNodeLocation(Node->GetGuid(), Location, InProgressMove);
}

FVector UHeartNodeLocationAccessorLibrary::GetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode* Node)
{
	return Accessor->GetNodeLocation3D(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode* Node, const FVector& Location, const bool InProgressMove)
{
	Accessor->SetNodeLocation3D(Node->GetGuid(), Location, InProgressMove);
}

namespace Heart::Features::Location
{
	FVector2D GetNodeLocation(const IHeartGraphNodeInterface& Node)
	{
		return Node.GetHeartGraph()->GetNodeLocationInterface()->GetNodeLocation(Node.GetNodeGuid());
	}
}