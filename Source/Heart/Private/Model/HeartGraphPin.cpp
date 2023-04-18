// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphPin.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"

UWorld* UHeartGraphPin::GetWorld() const
{
	if (!IsTemplate())
	{
		if (GetNode())
		{
			UWorld* NodeWorld = GetNode()->GetWorld();
			if (IsValid(NodeWorld))
			{
				return NodeWorld;
			}
		}
	}

	return Super::GetWorld();
}

bool UHeartGraphPin::ConnectTo(UHeartGraphPin* Other)
{
	if (!ensure(IsValid(Other)))
	{
		return false;
	}

	// Make sure we don't already link to it
	if (Links.Contains(Other->GetReference()))
	{
		return false;
	}

	UHeartGraphNode* MyNode = GetNode();

	if (!ensureMsgf(!Other->Links.Contains(GetReference()), TEXT("")))
	{
		return false;
	}

	if (!ensureMsgf(MyNode->GetGraph() == Other->GetNode()->GetGraph(), TEXT("")))
	{
		return false;
	}

	// Check that the other pin does not link to us
	//ensureMsgf(!Other->Links.Contains(this), TEXT("%s"), *GetLinkInfoString( LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("IsLinked", "is linked with pin").ToString(), ToPin));
	//ensureMsgf(MyNode->GetOuter() == ToPin->GetOwningNode()->GetOuter(), TEXT("%s"), *GetLinkInfoString( LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("OuterMismatch", "has a different outer than pin").ToString(), ToPin)); // Ensure both pins belong to the same graph

	// Add to both lists
	Links.Add(Other->GetReference());
	Other->Links.Add(GetReference());

#if WITH_EDITOR
	if (MyNode->GetEdGraphNode() && Other->GetNode()->GetEdGraphNode())
	{
		auto&& ThisEdGraphPin = MyNode->GetEdGraphNode()->FindPin(PinName);
		auto&& OtherEdGraphPin = Other->GetNode()->GetEdGraphNode()->FindPin(Other->PinName);

		if (ThisEdGraphPin && OtherEdGraphPin)
		{
			ThisEdGraphPin->MakeLinkTo(OtherEdGraphPin);
		}
	}
#endif

	OnPinConnectionsChanged.Broadcast(this);
	Other->OnPinConnectionsChanged.Broadcast(Other);
	return true;
}

void UHeartGraphPin::DisconnectFrom(const FHeartGraphPinReference Other, const bool NotifyNode)
{
	UHeartGraphPin* ToPin = ResolveConnectionByReference(Other);

	if (!IsValid(ToPin))
	{
		return;
	}

	// If we do indeed link to the passed in pin...
	if (Links.Contains(Other))
	{
		// Check that the other pin links to us
		//ensureAlwaysMsgf(ToPin->Links.Contains(this), TEXT("%s"), *GetLinkInfoString(LOCTEXT("BreakLinkTo", "BreakLinkTo").ToString(), LOCTEXT("NotLinked", "not reciprocally linked with pin").ToString(), ToPin));
		ToPin->Links.Remove(GetReference());
		Links.Remove(Other);

#if WITH_EDITOR
		if (GetNode()->GetEdGraphNode() && ToPin->GetNode()->GetEdGraphNode())
		{
			auto&& ThisEdGraphPin = GetNode()->GetEdGraphNode()->FindPin(PinName);
			auto&& OtherEdGraphPin = ToPin->GetNode()->GetEdGraphNode()->FindPin(ToPin->PinName);

			if (ThisEdGraphPin && OtherEdGraphPin)
			{
				ThisEdGraphPin->BreakLinkTo(OtherEdGraphPin);
			}
		}
#endif

		OnPinConnectionsChanged.Broadcast(this);
		ToPin->OnPinConnectionsChanged.Broadcast(ToPin);

		if (NotifyNode)
		{
			if (auto&& Node = ToPin->GetNode())
			{
				Node->NotifyPinConnectionsChanged(ToPin);
				GetNode()->GetGraph()->NotifyNodeConnectionsChanged({GetNode(), ToPin->GetNode()}, {this, ToPin});
			}
		}
	}
	else
	{
		// Check that the other pin does not link to us
		//ensureAlwaysMsgf(!ToPin->Links.Contains(GetReference()), TEXT("%s"), *GetLinkInfoString(LOCTEXT("MakeLinkTo", "MakeLinkTo").ToString(), LOCTEXT("IsLinked", "is linked with pin").ToString(), ToPin));
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

FName UHeartGraphPin::GetPinName() const
{
	return PinName;
}

const FText& UHeartGraphPin::GetFriendlyName() const
{
	return PinFriendlyName;
}

const FText& UHeartGraphPin::GetToolTip() const
{
	return PinTooltip;
}

FEdGraphPinType UHeartGraphPin::GetPinType() const
{
	static FEdGraphPinType DefaultEdGraphPinType = FEdGraphPinType("exec", NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());
	return DefaultEdGraphPinType;
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
