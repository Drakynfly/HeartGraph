// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/HeartGraphCanvasNode.h"

#include "Model/HeartNodeRegistrySubsystem.h"
#include "View/HeartGraphCanvasPin.h"
#include "View/HeartGraphCanvas.h"

UHeartWidgetInputLinker* UHeartGraphCanvasNode::ResolveLinker() const
{
	return GraphCanvas->ResolveLinker();
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
