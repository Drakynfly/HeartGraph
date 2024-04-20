// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartReplicatedData.h"
#include "GraphProxy/HeartGraphNetProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartReplicatedData)

void FHeartReplicatedFlake::PostReplicatedAdd(const FHeartReplicatedData& Array)
{
	Array.OwningProxy->PostReplicatedAdd(Array, *this);
}

void FHeartReplicatedFlake::PostReplicatedChange(const FHeartReplicatedData& Array)
{
	Array.OwningProxy->PostReplicatedChange(Array, *this);
}

void FHeartReplicatedFlake::PreReplicatedRemove(const FHeartReplicatedData& Array)
{
	Array.OwningProxy->PreReplicatedRemove(Array, *this);
}

void FHeartReplicatedFlake::PostReplicatedReceive(
	const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters)
{
}

FString FHeartReplicatedFlake::GetDebugString()
{
	return Guid.ToString();
}

int32 FHeartReplicatedData::IndexOf(const FHeartGuid& Guid) const
{
	// @todo replace with faster search than linear:
	// keep nodes sorted somehow and use binary search probably?
	return Items.IndexOfByPredicate(
		[Guid](const FHeartReplicatedFlake& Data)
		{
			return Data.Guid == Guid;
		});
}

void FHeartReplicatedData::Operate(const FHeartGuid& Guid, const TFunctionRef<void(FHeartReplicatedFlake&)>& Func)
{
	const int32 Index = IndexOf(Guid);

	if (Index != INDEX_NONE)
	{
		FHeartReplicatedFlake& Item = Items[Index];
		Func(Item);
		MarkItemDirty(Item);
	}
	else
	{
		FHeartReplicatedFlake& Item = Items.AddDefaulted_GetRef();
		Item.Guid = Guid;
		Func(Item);
		MarkItemDirty(Item);
	}
}

bool FHeartReplicatedData::Delete(const FHeartGuid& Guid)
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