// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartPinConnectionEdit.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

namespace Heart::Connections
{
	FEdit::FEdit(UHeartGraphNode* Node)
	  : Graph(Node->GetGraph()) {}

	FEdit::~FEdit()
	{
		if (!Modified())
		{
			return;
		}

		FHeartGraphConnectionEvent Event;

		for (auto&& Element : ChangedPins)
		{
			Event.AffectedNodes.Add(Element.Key);
			Event.AffectedPins.Add(Element.Value);
			Element.Key->NotifyPinConnectionsChanged(Element.Value);
		}

		Graph->HandleGraphConnectionEvent(Event);
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
		ANode->PinData.AddConnection(PinA.PinGuid, PinB);
		BNode->PinData.AddConnection(PinB.PinGuid, PinA);

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

		if (auto&& Connections = Node->PinData.ViewConnections(Pin.PinGuid);
			Connections.IsValid())
		{
			for (const TArray<FHeartGraphPinReference> ConnectionsCopy(Connections.Get().GetLinks());
				 const FHeartGraphPinReference& Link : ConnectionsCopy)
			{
				UHeartGraphNode* BNode = Graph->GetNode(Link.NodeGuid);
				Internal_Disconnect(Node, Pin, BNode, Link);
			}
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

		for (auto ConnectionsCopy = Node->PinData.PinConnections;
			 auto&& Element : ConnectionsCopy)
		{
			const FHeartGraphPinReference This = {NodeGuid, Element.Key};
			for (const FHeartGraphPinReference& Link : Element.Value)
			{
				UHeartGraphNode* BNode = Graph->GetNode(Link.NodeGuid);
				Internal_Disconnect(Node, This, BNode, Link);
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

	FEdit& FEdit::CreateMementos(const FHeartGraphPinReference& Pin, TMap<FHeartNodeGuid, FMemento>& OutMementos)
	{
		const UHeartGraphNode* Node = Graph->GetNode(Pin.NodeGuid);
		if (!ensure(IsValid(Node)))
		{
			return *this;
		}

		// Memento for self
		OutMementos.Add(Pin.NodeGuid).PinConnections = Node->PinData.PinConnections;

		// Mementos for all connected pins
		if (auto&& Connections = Node->PinData.ViewConnections(Pin.PinGuid);
			Connections.IsValid())
		{
			for (const FHeartGraphPinReference& Link : Connections.Get())
			{
				OutMementos.Add(Link.NodeGuid).PinConnections = Graph->GetNode(Link.NodeGuid)->PinData.PinConnections;
			}
		}

		return *this;
	}

	FEdit& FEdit::CreateAllMementos(const FHeartNodeGuid& NodeGuid, TMap<FHeartNodeGuid, FMemento>& OutMementos)
	{
		const UHeartGraphNode* Node = Graph->GetNode(NodeGuid);
		if (!ensure(IsValid(Node)))
		{
			return *this;
		}

		// Memento for this pin
		OutMementos.Add(NodeGuid).PinConnections = Node->PinData.PinConnections;

		for (auto&& Connections = Node->PinData.PinConnections;
			 auto&& Element : Connections)
		{
			// Mementos for all connected pins
			for (const FHeartGraphPinReference& Link : Element.Value)
			{
				OutMementos.Add(Link.NodeGuid).PinConnections = Graph->GetNode(Link.NodeGuid)->PinData.PinConnections;
			}
		}

		return *this;
	}

	FEdit& FEdit::RestoreMementos(const TMap<FHeartNodeGuid, FMemento>& Mementos)
	{
		for (auto&& PinAndMemento : Mementos)
		{
			UHeartGraphNode* ANode = Graph->GetNode(PinAndMemento.Key);
			if (!ensure(IsValid(ANode)))
			{
				return *this;
			}

			// Mark all pins as changed, we have no idea what the memento will remove.
			for (auto&& Element : ANode->PinData.PinConnections)
			{
				ChangedPins.Add(ANode, Element.Key);
			}

			ANode->PinData.PinConnections = PinAndMemento.Value.PinConnections;

			// Mark all pins as changed again, as we have no idea what the memento has restored.
			for (auto&& Element : ANode->PinData.PinConnections)
			{
				ChangedPins.Add(ANode, Element.Key);
			}
		}
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