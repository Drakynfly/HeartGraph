// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartPinConnectionEdit.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinReference.h"

namespace Heart::Connections
{
	FEdit::~FEdit()
	{
		if (!Modified())
		{
			return;
		}

		FHeartGraphConnectionEvent Event;

		for (auto Element : ChangedPins)
		{
			Event.AffectedNodes.Add(Element.Key);
			Event.AffectedPins.Add(Element.Value);
			Element.Key->NotifyPinConnectionsChanged(Element.Value);
		}

		Graph->NotifyNodeConnectionsChanged(Event);
	}

	FEdit& FEdit::Connect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
	{
		UHeartGraphNode* ANode = Graph->GetNode(PinA.NodeGuid);
		UHeartGraphNode* BNode = Graph->GetNode(PinB.NodeGuid);

		if (!ensure(IsValid(ANode) && IsValid(BNode)))
		{
			return *this;
		}

		// Add to both lists
		ANode->PinData.GetConnectionsMutable(PinA.PinGuid).Links.Add(PinB);
		BNode->PinData.GetConnectionsMutable(PinB.PinGuid).Links.Add(PinA);

		ChangedPins.Add(ANode, PinA.PinGuid);
		ChangedPins.Add(BNode, PinB.PinGuid);

		return *this;
	}

	FEdit& FEdit::Disconnect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
	{
		UHeartGraphNode* ANode = Graph->GetNode(PinA.NodeGuid);
		UHeartGraphNode* BNode = Graph->GetNode(PinB.NodeGuid);

		Internal_Disconnect(ANode, PinA, BNode, PinB);

		return *this;
	}

	FEdit& FEdit::DisconnectAll(const FHeartGraphPinReference& Pin)
	{
		UHeartGraphNode* Node = Graph->GetNode(Pin.NodeGuid);

		if (!ensure(IsValid(Node)))
		{
			return *this;
		}

		for (const FHeartGraphPinConnections Connections = Node->PinData.GetConnections(Pin.PinGuid);
			const FHeartGraphPinReference& Link : Connections.Links)
		{
			UHeartGraphNode* BNode = Graph->GetNode(Link.NodeGuid);
			Internal_Disconnect(Node, Pin, BNode, Link);
		}

		return *this;
	}

	FEdit& FEdit::DisconnectAll(const FHeartNodeGuid& NodeGuid)
	{
		UHeartGraphNode* Node = Graph->GetNode(NodeGuid);

		if (!ensure(IsValid(Node)))
		{
			return *this;
		}

		for (auto Connections = Node->PinData.PinConnections;
			 auto&& Element : Connections)
		{
			for (const FHeartGraphPinReference& Link : Element.Value.Links)
			{
				UHeartGraphNode* BNode = Graph->GetNode(Link.NodeGuid);
				Internal_Disconnect(Node, {NodeGuid, Element.Key}, BNode, Link);
			}
		}

		return *this;
	}

	FEdit& FEdit::Override(const FHeartGraphPinReference& Pin, const FHeartGraphPinConnections& Connections)
	{
		UHeartGraphNode* ANode = Graph->GetNode(Pin.NodeGuid);

		if (!ensure(IsValid(ANode)))
		{
			return *this;
		}

		ANode->PinData.GetConnectionsMutable(Pin.PinGuid) = Connections;

		ChangedPins.Add(ANode, Pin.PinGuid);

		return *this;
	}

	void FEdit::Internal_Disconnect(UHeartGraphNode* NodeA, const FHeartGraphPinReference& PinA,
									UHeartGraphNode* NodeB, const FHeartGraphPinReference& PinB)
	{
		if (IsValid(NodeA))
		{
			if (NodeA->PinData.RemoveConnection(PinA.PinGuid, PinB))
			{
				ChangedPins.Add(NodeA, PinA.PinGuid);
			}
		}

		if (IsValid(NodeB))
		{
			if (NodeB->PinData.RemoveConnection(PinB.PinGuid, PinA))
			{
				ChangedPins.Add(NodeB, PinB.PinGuid);
			}
		}
	}
}