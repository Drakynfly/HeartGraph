// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasNode.h"

#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvas.h"

#include "Model/HeartGraph.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "UMG/HeartGraphCanvasConnection.h"

void UHeartGraphCanvasNode::NativeDestruct()
{
	if (GraphNode.IsValid())
	{
		GraphNode->OnPinConnectionsChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

UHeartWidgetInputLinker* UHeartGraphCanvasNode::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvas.Get());
}

UHeartGraphNode* UHeartGraphCanvasNode::GetHeartGraphNode() const
{
	return GraphNode.Get();
}

void UHeartGraphCanvasNode::PostInitNode()
{
	if (GraphNode.IsValid())
	{
		GraphNode->OnPinConnectionsChanged.AddDynamic(this, &ThisClass::RebuildPinConnections);
	}
}

void UHeartGraphCanvasNode::SetNodeSelectedFromGraph(const bool Selected)
{
	if (NodeSelected != Selected)
	{
		NodeSelected = Selected;
		OnNodeSelectionChanged();
	}
}

void UHeartGraphCanvasNode::SetNodeSelected(const bool Selected)
{
	if (Selected)
	{
		GraphCanvas->SelectNode(GraphNode->GetGuid());
	}
	else
	{
		GraphCanvas->UnselectNode(GraphNode->GetGuid());
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

void UHeartGraphCanvasNode::RebuildPinConnections(FHeartPinGuid Pin)
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
			return PinWidget->GetPinGuid() == Pin;
		});

	if (!ThisPin)
	{
		return;
	}

	auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();
	auto&& CanvasGraphClass = GetCanvas()->GetGraph()->GetClass();
	auto&& CanvasGraphRegistry = RegistrySubsystem->GetRegistry(CanvasGraphClass);

	const FHeartGraphPinDesc ThisDesc = GetGraphNode()->GetPinDesc(Pin);

	TSet<FHeartGraphPinReference> Connections = GetGraphNode()->GetLinks(Pin).Links;
	for (const FHeartGraphPinReference& Connection : Connections)
	{
		const FHeartGraphPinDesc ConnectionDesc = GetGraphNode()->GetPinDesc(Connection.PinGuid);

		auto&& ConnectionVisualizer = CanvasGraphRegistry->GetVisualizerClassForGraphConnection(ThisDesc, ConnectionDesc, UWidget::StaticClass());
		if (!IsValid(ConnectionVisualizer))
		{
			continue;
		}

		UHeartGraphCanvasNode* ConnectedNode = GraphCanvas->GetCanvasNode(Connection.NodeGuid);
		if (!ConnectedNode)
		{
			continue;
		}

		auto&& ConnectedPin = ConnectedNode->PinWidgets.FindByPredicate(
			[this, Connection](const TObjectPtr<UHeartGraphCanvasPin>& PinWidget)
			{
				return PinWidget->GraphPin == Connection.PinGuid;
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

UHeartGraphCanvasPin* UHeartGraphCanvasNode::GetPinWidget(const FHeartPinGuid Guid) const
{
	for (auto&& PinWidget : PinWidgets)
	{
		if (PinWidget->GetPinGuid() == Guid)
		{
			return PinWidget;
		}
	}

	return nullptr;
}

UHeartGraphCanvasPin* UHeartGraphCanvasNode::CreatePinWidget(const FHeartPinGuid Pin)
{
	const FHeartGraphPinDesc Desc = GraphNode->GetPinDesc(Pin);

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();
	UClass* CanvasGraphClass = GetCanvas()->GetGraph()->GetClass();
	auto&& CanvasGraphRegistry = NodeRegistrySubsystem->GetRegistry(CanvasGraphClass);
	UClass* PinVisualizer = CanvasGraphRegistry->GetVisualizerClassForGraphPin(Desc, UHeartGraphCanvasPin::StaticClass());

	if (IsValid(PinVisualizer))
	{
		if (auto&& PinWidget = CreateWidget<UHeartGraphCanvasPin>(this, PinVisualizer))
		{
			PinWidget->GraphCanvasNode = this;
			PinWidget->GraphPin = Pin;
			PinWidget->PinDescription = Desc;
			PinWidgets.Add(PinWidget);
			return PinWidget;
		}
	}

	return nullptr;
}

void UHeartGraphCanvasNode::DestroyPinWidget(UHeartGraphCanvasPin* PinWidget)
{
	ConnectionWidgets.RemoveAll(
		[PinWidget](const TObjectPtr<UHeartGraphCanvasConnection>& ConnectionWidget)
		{
			return ConnectionWidget->FromPin == PinWidget;
		});

	PinWidget->RemoveFromParent();
	PinWidgets.Remove(PinWidget);
}
