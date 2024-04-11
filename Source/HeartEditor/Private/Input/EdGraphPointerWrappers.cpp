// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "EdGraphPointerWrappers.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EdGraphPointerWrappers)

UHeartEdGraphPin* UHeartEdGraphPin::Wrap(const UEdGraphPin* Pin)
{
	UHeartEdGraphPin* NewWrapper = NewObject<UHeartEdGraphPin>();
	NewWrapper->EdGraphPin = Pin;
	if (auto Node = NewWrapper->GetHeartGraphNode();
		IsValid(Node))
	{
		if (auto Option = Node->QueryPins().Find(
				[&Pin](const FHeartGraphPinDesc& Desc)
				{
					return Desc.Name == Pin->PinName;
				});
			Option.IsSet())
		{
			NewWrapper->PinGuid = Option.GetValue();
		}
	}

	return NewWrapper;
}

UHeartGraphNode* UHeartEdGraphPin::GetHeartGraphNode() const
{
	if (EdGraphPin)
	{
		return Cast<UHeartEdGraphNode>(EdGraphPin->GetOwningNode())->GetHeartGraphNode();
	}
	return nullptr;
}

FHeartPinGuid UHeartEdGraphPin::GetPinGuid() const
{
	return PinGuid;
}