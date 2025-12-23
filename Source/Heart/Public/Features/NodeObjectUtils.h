// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class IHeartGraphNodeInterface;

namespace Heart::Features::NodeObject
{
	HEART_API UObject* GetNodeObject(const IHeartGraphNodeInterface& NodeInterface);

	template <typename T>
	T* GetTypedNodeObject(const IHeartGraphNodeInterface& NodeInterface)
	{
		return Cast<T>(GetNodeObject(NodeInterface));
	}
}
