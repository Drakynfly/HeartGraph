// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasPin)

UHeartInputLinkerBase* UHeartGraphCanvasPin::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvasNode.Get());
}

UHeartGraphNode* UHeartGraphCanvasPin::GetHeartGraphNode() const
{
	return GraphCanvasNode.IsValid() ? GraphCanvasNode->GetGraphNode() : nullptr;
}

FHeartPinGuid UHeartGraphCanvasPin::GetPinGuid() const
{
	return GraphPin;
}

void UHeartGraphCanvasPin::SetIsPreviewConnectionTarget(const bool IsTarget, const bool CanConnect)
{
	DisplayPreviewConnectionTarget(IsTarget, CanConnect);
}

FHeartGraphPinReference UHeartGraphCanvasPin::GetPinReference() const
{
	return { GraphCanvasNode->GetGraphNode()->GetGuid(), GraphPin };
}

UHeartGraphCanvas* UHeartGraphCanvasPin::GetCanvas() const
{
	if (GraphCanvasNode.IsValid())
	{
		return GraphCanvasNode.Get()->GetCanvas();
	}

	return nullptr;
}