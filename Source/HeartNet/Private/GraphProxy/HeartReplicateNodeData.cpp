// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartReplicateNodeData.h"
#include "GraphProxy/HeartGraphNetProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartReplicateNodeData)

void FHeartReplicatedNodeData::PostReplicatedAdd(const FHeartReplicatedGraphNodes& Array)
{
	Array.OwningProxy->UpdateNodeProxy(*this, Heart::Net::Tags::Node_Added);
}

void FHeartReplicatedNodeData::PostReplicatedChange(const FHeartReplicatedGraphNodes& Array)
{
	// @todo clients have no idea what kind of changes the server is making...
	Array.OwningProxy->UpdateNodeProxy(*this, Heart::Net::Tags::Other);
}

void FHeartReplicatedNodeData::PreReplicatedRemove(const FHeartReplicatedGraphNodes& Array)
{
	Array.OwningProxy->RemoveNodeProxy(Data.Guid);
}

void FHeartReplicatedNodeData::PostReplicatedReceive(
	const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters)
{
}

FString FHeartReplicatedNodeData::GetDebugString()
{
	return Data.Guid.ToString();
}

int32 FHeartReplicatedGraphNodes::IndexOf(const FHeartNodeGuid& Guid) const
{
	// @todo replace with faster search than linear:
	// keep nodes sorted somehow and use binary search probably?
	return Items.IndexOfByPredicate(
		[Guid](const FHeartReplicatedNodeData& NodeData)
		{
			return NodeData.Data.Guid == Guid;
		});
}

void FHeartReplicatedGraphNodes::Operate(const FHeartNodeGuid& Guid, const TFunctionRef<void(FHeartReplicatedNodeData&)>& Func)
{
	const int32 Index = IndexOf(Guid);

	if (Index != INDEX_NONE)
	{
		FHeartReplicatedNodeData& Item = Items[Index];
		Func(Item);
		MarkItemDirty(Item);
	}
	else
	{
		FHeartReplicatedNodeData& Item = Items.AddDefaulted_GetRef();
		Item.Data.Guid = Guid;
		Func(Item);
		MarkItemDirty(Item);
	}
}

bool FHeartReplicatedGraphNodes::Delete(const FHeartNodeGuid& Guid)
{
	const int32 Index = IndexOf(Guid);

	if (Index != INDEX_NONE)
	{
		Items.RemoveAt(Index);
		MarkArrayDirty();
		return true;
	}

	return false;
}