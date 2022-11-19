// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvas.h"
#include "Model/HeartNodeRegistrySubsystem.h"


UHeartWidgetInputLinker* UHeartGraphCanvasNode::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvas.Get());
}

void UHeartGraphCanvasNode::SetNodeSelected(const bool Selected)
{
	if (NodeSelected != Selected)
	{
		NodeSelected = Selected;
		OnNodeSelectionChanged();
	}
}

UHeartGraphCanvasPin* UHeartGraphCanvasNode::GetPinWidget(const FHeartPinGuid& Guid) const
{
	for (auto&& PinWidget : PinWidgets)
	{
		if (PinWidget->GetPin()->GetGuid() == Guid)
		{
			return PinWidget;
		}
	}

	return nullptr;
}

UHeartGraphCanvasPin* UHeartGraphCanvasNode::CreatePinWidget(UHeartGraphPin* Pin)
{
	check(Pin);

	UHeartNodeRegistrySubsystem* NodeRegistrySubsystem = GetGameInstance()->GetSubsystem<UHeartNodeRegistrySubsystem>();

	auto&& VisualizerClass =
		NodeRegistrySubsystem->GetRegistry(GetCanvas()->GetGraph()->GetClass())->GetVisualizerClassForGraphPin(Pin->GetClass());

	if (VisualizerClass->IsChildOf<UHeartGraphCanvasPin>())
	{
		if (auto&& Widget = CreateWidget<UHeartGraphCanvasPin>(this, VisualizerClass))
		{
			Widget->GraphCanvasNode = this;
			Widget->GraphPin = Pin;
			return Widget;
		}
	}

	return nullptr;
}
