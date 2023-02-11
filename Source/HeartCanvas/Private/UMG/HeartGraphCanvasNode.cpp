// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasNode.h"

#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvas.h"

#include "Model/HeartGraph.h"

#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "UMG/HeartGraphCanvasConnection.h"

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

void UHeartGraphCanvasNode::RebuildAllPinConnections()
{
	auto&& OutputPins = GraphNode->GetOutputPins();
	for (auto&& OutputPin : OutputPins)
	{
		RebuildPinConnections(OutputPin);
	}
}

void UHeartGraphCanvasNode::RebuildPinConnections(UHeartGraphPin* Pin)
{
	for (auto&& ConnectionWidget : ConnectionWidgets)
	{
		ConnectionWidget->RemoveFromParent();
	}
	TArray<TObjectPtr<UHeartGraphCanvasConnection>> RebuildPool = ConnectionWidgets;
	ConnectionWidgets.Empty();

	auto GetConnectionWidget = [this, &RebuildPool](UClass* ConnectionVisualizer)
	{
		auto&& FoundPoolWidget = RebuildPool.FindByPredicate(
			[ConnectionVisualizer](const TObjectPtr<UHeartGraphCanvasConnection>& PoolWidget)
			{
				return PoolWidget->GetClass() == ConnectionVisualizer;
			});

		if (FoundPoolWidget != nullptr && IsValid(*FoundPoolWidget))
		{
			return FoundPoolWidget->Get();
		}

		return CreateWidget<UHeartGraphCanvasConnection>(this, ConnectionVisualizer);
	};

	auto&& ThisPin = PinWidgets.FindByPredicate(
		[Pin](const TObjectPtr<UHeartGraphCanvasPin>& PinWidget)
		{
			return PinWidget->GetPin() == Pin;
		});

	if (!ThisPin)
	{
		return;
	}

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>();
	auto&& CanvasGraphClass = GetCanvas()->GetGraph()->GetClass();
	auto&& CanvasGraphRegistry = NodeRegistrySubsystem->GetRegistry(CanvasGraphClass);

	TArray<UHeartGraphPin*> Connections = Pin->GetAllConnections();
	for (auto&& Connection : Connections)
	{
		auto&& ConnectionVisualizer = CanvasGraphRegistry->GetVisualizerClassForGraphConnection(Pin->GetClass(), Connection->GetClass(), UWidget::StaticClass());
		if (!IsValid(ConnectionVisualizer))
		{
			continue;
		}

		UHeartGraphCanvasNode* ConnectedNode = GraphCanvas->GetCanvasNode(Connection->GetNode()->GetGuid());
		if (!ConnectedNode)
		{
			continue;
		}

		auto&& ConnectedPin = ConnectedNode->PinWidgets.FindByPredicate(
			[this, Connection](const TObjectPtr<UHeartGraphCanvasPin>& PinWidget)
			{
				return PinWidget->GraphPin == Connection;
			});

		if (!ConnectedPin)
		{
			continue;
		}

		if (auto&& ConnectionWidget = GetConnectionWidget(ConnectionVisualizer))
		{
			ConnectionWidget->GraphCanvas = GraphCanvas;
			ConnectionWidget->FromPin = ThisPin->Get();
			ConnectionWidget->ToPin = ConnectedPin->Get();
			ConnectionWidgets.Add(ConnectionWidget);
			GraphCanvas->AddConnectionWidget(ConnectionWidget);
		}
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

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>();
	auto&& CanvasGraphClass = GetCanvas()->GetGraph()->GetClass();
	auto&& CanvasGraphRegistry = NodeRegistrySubsystem->GetRegistry(CanvasGraphClass);
	auto&& PinVisualizer = CanvasGraphRegistry->GetVisualizerClassForGraphPin(Pin->GetClass(), UWidget::StaticClass());

	if (PinVisualizer->IsChildOf<UHeartGraphCanvasPin>())
	{
		if (auto&& PinWidget = CreateWidget<UHeartGraphCanvasPin>(this, PinVisualizer))
		{
			PinWidget->GraphCanvasNode = this;
			PinWidget->GraphPin = Pin;
			PinWidgets.Add(PinWidget);

			if (Pin->GetDirection() == EHeartPinDirection::Output)
			{
				Pin->OnPinConnectionsChanged.AddDynamic(this, &ThisClass::RebuildPinConnections);
			}

			return PinWidget;
		}
	}

	return nullptr;
}

void UHeartGraphCanvasNode::DestroyPinWidget(UHeartGraphCanvasPin* PinWidget)
{
	if (auto&& Pin = PinWidget->GetPin())
	{
		Pin->OnPinConnectionsChanged.RemoveAll(this);
	}

	ConnectionWidgets.RemoveAll(
		[PinWidget](const TObjectPtr<UHeartGraphCanvasConnection>& ConnectionWidget)
		{
			return ConnectionWidget->FromPin == PinWidget;
		});

	PinWidget->RemoveFromParent();
	PinWidgets.Remove(PinWidget);
}
