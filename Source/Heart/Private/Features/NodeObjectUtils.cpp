// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Features/NodeObjectUtils.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"

namespace Heart::Features::NodeObject
{
	UObject* GetNodeObject(const IHeartGraphNodeInterface& NodeInterface)
	{
		if (const UHeartGraph* Graph = NodeInterface.GetHeartGraph())
		{
			return Graph->GetNode(NodeInterface.GetNodeGuid())->GetNodeObject();
		}
		return nullptr;
	}
}
