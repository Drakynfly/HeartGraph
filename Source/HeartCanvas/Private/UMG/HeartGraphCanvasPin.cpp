// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "Model/HeartGraphNode.h"

UHeartWidgetInputLinker* UHeartGraphCanvasPin::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvasNode.Get());
}

UHeartGraphNode* UHeartGraphCanvasPin::GetNode_Implementation()
{
	return GraphCanvasNode.IsValid() ? GraphCanvasNode->GetNode() : nullptr;
}

FHeartPinGuid UHeartGraphCanvasPin::GetPinGuid_Implementation() const
{
	return GraphPin;
}

void UHeartGraphCanvasPin::SetIsPreviewConnectionTarget(const bool IsTarget, const bool CanConnect)
{
	DisplayPreviewConnectionTarget(IsTarget, CanConnect);
}

FHeartGraphPinReference UHeartGraphCanvasPin::GetPinReference() const
{
	return { GraphCanvasNode->GetNode()->GetGuid(), GraphPin };
}

UHeartGraphCanvas* UHeartGraphCanvasPin::GetCanvas() const
{
	if (GraphCanvasNode.IsValid())
	{
		return GraphCanvasNode.Get()->GetCanvas();
	}

	return nullptr;
}
