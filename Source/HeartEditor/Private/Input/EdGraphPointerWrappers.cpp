// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "EdGraphPointerWrappers.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EdGraphPointerWrappers)

UHeartEdGraphPin* UHeartEdGraphPin::Wrap(const UEdGraphPin* Pin)
{
	const UHeartEdGraphNode* EdGraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode());

	UHeartEdGraphPin* NewWrapper = NewObject<UHeartEdGraphPin>();
	NewWrapper->EdGraphPin = Pin;
	NewWrapper->PinGuid = EdGraphNode->GetHeartGraph()->FindNodePin(EdGraphNode->GetNodeGuid(), Pin->PinName);

	return NewWrapper;
}

UHeartGraph* UHeartEdGraphPin::GetHeartGraph() const
{
	if (EdGraphPin)
	{
		return Cast<UHeartEdGraphNode>(EdGraphPin->GetOwningNode())->GetHeartGraph();
	}
	return nullptr;
}

FHeartNodeGuid UHeartEdGraphPin::GetNodeGuid() const
{
	if (EdGraphPin)
	{
		return Cast<UHeartEdGraphNode>(EdGraphPin->GetOwningNode())->GetNodeGuid();
	}
	return FHeartNodeGuid();
}

FHeartPinGuid UHeartEdGraphPin::GetPinGuid() const
{
	return PinGuid;
}