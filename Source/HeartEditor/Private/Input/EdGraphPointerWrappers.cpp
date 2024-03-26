// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "EdGraphPointerWrappers.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EdGraphPointerWrappers)

UHeartEdGraphPin* UHeartEdGraphPin::Wrap(const UEdGraphPin* Pin)
{
	UHeartEdGraphPin* NewWrapper = NewObject<UHeartEdGraphPin>();
	NewWrapper->EdGraphPin = Pin;
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