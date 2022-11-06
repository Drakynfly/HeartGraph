// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphPin.h"
#include "ModelView/HeartGraphNode.h"
#include "Model/HeartGraph.h"

UWorld* UHeartGraphPin::GetWorld() const
{
	if (GetNode())
	{
		return GetNode()->GetWorld();
	}
	return nullptr;
}

void UHeartGraphPin::NewGuid()
{
	Guid = FHeartPinGuid::NewGuid();
}

void UHeartGraphPin::SetDirection(EHeartPinDirection Direction)
{
	PinDirection = Direction;
}

void UHeartGraphPin::ConnectTo(UHeartGraphPin* Other)
{
	if (Other)
	{
		// Make sure we don't already link to it
		if (!Links.Contains(Other->GetReference()))
		{
			UHeartGraphNode* MyNode = GetNode();

			if (!ensureMsgf(!Other->Links.Contains(GetReference()), TEXT("")))
			{
				return;
			}

			if (!ensureMsgf(MyNode->GetGraph() == Other->GetNode()->GetGraph(), TEXT("")))
			{
				return;
			}

			// Check that the other pin does not link to us
			//ensureMsgf(!Other->Links.Contains(this), TEXT("%s"), *GetLinkInfoString( LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("IsLinked", "is linked with pin").ToString(), ToPin));
			//ensureMsgf(MyNode->GetOuter() == ToPin->GetOwningNode()->GetOuter(), TEXT("%s"), *GetLinkInfoString( LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("OuterMismatch", "has a different outer than pin").ToString(), ToPin)); // Ensure both pins belong to the same graph

			// Add to both lists
			Links.Add(Other->GetReference());
			Other->Links.Add(GetReference());

			OnPinConnectionsChanged.Broadcast(this);
			Other->OnPinConnectionsChanged.Broadcast(Other);
		}
	}
}

void UHeartGraphPin::DisconnectFrom(const FHeartGraphPinReference Other, const bool NotifyNode)
{
	UHeartGraphPin* ToPin = ResolveConnectionByReference(Other);

	if (ToPin)
	{
		// If we do indeed link to the passed in pin...
		if (Links.Contains(Other))
		{
			// Check that the other pin links to us
			//ensureAlwaysMsgf(ToPin->Links.Contains(this), TEXT("%s"), *GetLinkInfoString(LOCTEXT("BreakLinkTo", "BreakLinkTo").ToString(), LOCTEXT("NotLinked", "not reciprocally linked with pin").ToString(), ToPin));
			ToPin->Links.Remove(GetReference());
			Links.Remove(Other);

			OnPinConnectionsChanged.Broadcast(this);
			ToPin->OnPinConnectionsChanged.Broadcast(ToPin);

			if (NotifyNode)
			{
				if (auto&& Node = ToPin->GetNode())
				{
					Node->NotifyPinConnectionsChanged(ToPin);
				}
			}
		}
		else
		{
			// Check that the other pin does not link to us
			//ensureAlwaysMsgf(!ToPin->Links.Contains(GetReference()), TEXT("%s"), *GetLinkInfoString(LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("IsLinked", "is linked with pin").ToString(), ToPin));
		}
	}
}

void UHeartGraphPin::DisconnectFromAll(const bool NotifyNodes)
{
	auto LinksCopy = Links;

	for (auto&& Link : LinksCopy)
	{
		DisconnectFrom(Link, NotifyNodes);
	}
}

UHeartGraphNode* UHeartGraphPin::GetNode() const
{
	return GetOwningNode<UHeartGraphNode>();
}

FHeartGraphPinReference UHeartGraphPin::GetReference() const
{
	check(Guid.IsValid());
	return { GetNode()->GetGuid(), Guid };
}

TArray<UHeartGraphPin*> UHeartGraphPin::GetAllConnections()
{
	TArray<UHeartGraphPin*> OutConnections;

	if (auto&& Graph = GetNode()->GetGraph())
	{
		for (auto&& Link : Links)
		{
			if (auto&& Node = Graph->GetNode(Link.NodeGuid))
			{
				if (auto&& Pin = Node->GetPin(Link.PinGuid))
				{
					OutConnections.Add(Pin);
				}
			}
		}
	}

	return OutConnections;
}

UHeartGraphPin* UHeartGraphPin::ResolveConnectionByReference(const FHeartGraphPinReference Reference) const
{
	if (!ensure(Links.Contains(Reference)))
	{
		return nullptr;
	}

	if (auto&& Graph = GetNode()->GetGraph())
	{
		if (auto&& Node = Graph->GetNode(Reference.NodeGuid))
		{
			if (auto&& Pin = Node->GetPin(Reference.PinGuid))
			{
				return Pin;
			}
		}
	}

	return nullptr;
}

UHeartGraphPin* UHeartGraphPin::ResolveConnection(const int32 Index) const
{
	if (!ensure(Links.IsValidIndex(Index)))
	{
		return nullptr;
	}

	auto&& PinRef = Links[Index];

	if (auto&& Graph = GetNode()->GetGraph())
	{
		if (auto&& Node = Graph->GetNode(PinRef.NodeGuid))
		{
			if (auto&& Pin = Node->GetPin(PinRef.PinGuid))
			{
				return Pin;
			}
		}
	}

	return nullptr;
}
