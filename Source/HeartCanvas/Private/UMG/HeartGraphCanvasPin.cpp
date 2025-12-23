// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "Model/HeartGraphNode.h"
#include "UMG/HeartGraphCanvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasPin)

UHeartInputLinkerBase* UHeartGraphCanvasPin::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvasNode.Get());
}

UHeartGraph* UHeartGraphCanvasPin::GetHeartGraph() const
{
	return GraphCanvasNode.IsValid() ? GraphCanvasNode->GetCanvas()->GetGraph() : nullptr;
}

FHeartNodeGuid UHeartGraphCanvasPin::GetNodeGuid() const
{
	return GraphCanvasNode.IsValid() ? GraphCanvasNode->GetGraphNode()->GetGuid() : FHeartNodeGuid();
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