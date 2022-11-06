// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/HeartGraphCanvasPin.h"
#include "View/HeartGraphCanvasNode.h"

UHeartWidgetInputLinker* UHeartGraphCanvasPin::ResolveLinker() const
{
	return GraphCanvasNode->ResolveLinker();
}

void UHeartGraphCanvasPin::SetIsPreviewConnectionTarget(const bool Value, bool CanConnect)
{
	DisplayPreviewConnectionTarget(Value, CanConnect);
}

UHeartGraphCanvas* UHeartGraphCanvasPin::GetCanvas() const
{
	if (GraphCanvasNode.IsValid())
	{
		return GraphCanvasNode.Get()->GetCanvas();
	}

	return nullptr;
}
