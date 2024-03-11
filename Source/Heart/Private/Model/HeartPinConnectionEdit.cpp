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

		if (!ensureAlways(IsValid(ANode) && IsValid(BNode)))
		{
			return *this;
		}

		Internal_Disconnect(ANode, PinA.PinGuid, BNode, PinB.PinGuid);

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
			Internal_Disconnect(Node, Pin.PinGuid, BNode, Link.PinGuid);
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

		for (auto Element : Node->PinData.PinConnections)
		{
			for (const FHeartGraphPinReference& Link : Element.Value.Links)
			{
				UHeartGraphNode* BNode = Graph->GetNode(Link.NodeGuid);
				Internal_Disconnect(Node, Element.Key, BNode, Link.PinGuid);
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

	void FEdit::Internal_Disconnect(UHeartGraphNode* NodeA, const FHeartPinGuid PinA,
									UHeartGraphNode* NodeB, const FHeartPinGuid PinB)
	{
		auto&& ConnectionsA = NodeA->PinData.GetConnectionsMutable(PinA).Links;
		auto&& ConnectionsB = NodeB->PinData.GetConnectionsMutable(PinB).Links;

		bool Removed = false;

		Removed |= !!ConnectionsA.Remove({NodeB->Guid, PinB});
		Removed |= !!ConnectionsB.Remove({NodeA->Guid, PinA});

		// We would assume that both of these are true, but proceed anyway if only one of them are...
		if (Removed)
		{
			ChangedPins.Add(NodeA, PinA);
			ChangedPins.Add(NodeB, PinB);
		}
	}
}