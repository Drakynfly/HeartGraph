// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasNode.h"

#include "HeartCanvasPrivate.h"
#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvas.h"

#include "Model/HeartGraph.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "UMG/HeartGraphCanvasConnection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasNode)

DECLARE_CYCLE_STAT(TEXT("RebuildPinConnections"), STAT_RebuildPinConnections, STATGROUP_HeartCanvas);

void UHeartGraphCanvasNode::NativeDestruct()
{
	for (const auto Element : ConnectionWidgets)
	{
		Element->RemoveFromParent();
	}

	if (GraphNode.IsValid())
	{
		GraphNode->OnPinConnectionsChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

UHeartWidgetInputLinker* UHeartGraphCanvasNode::ResolveLinker_Implementation() const
{
	return GraphCanvas.IsValid() ? Execute_ResolveLinker(GraphCanvas.Get()) : nullptr;
}

UHeartGraphNode* UHeartGraphCanvasNode::GetHeartGraphNode() const
{
	return GraphNode.Get();
}

void UHeartGraphCanvasNode::PostInitNode()
{
	if (GraphNode.IsValid())
	{
		RebuildAllPinConnections();
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

void UHeartGraphCanvasNode::RebuildPinConnections(const FHeartPinGuid& Pin)
{
	SCOPE_CYCLE_COUNTER(STAT_RebuildPinConnections)

	TArray<TObjectPtr<UHeartGraphCanvasConnection>> RebuildPool;

	for (auto It = ConnectionWidgets.CreateIterator(); It; ++It)
	{
		TObjectPtr<UHeartGraphCanvasConnection> ConnectionWidget = *It;

		if (ConnectionWidget && ConnectionWidget->FromPin.PinGuid == Pin)
		{
			ConnectionWidget->RemoveFromParent();
			RebuildPool.Add(ConnectionWidget);
			It.RemoveCurrent();
		}
	}

	const FHeartGraphPinDesc& ThisDesc = GraphNode->GetPinDesc(Pin);

	if (ThisDesc.Direction != EHeartPinDirection::Output) return;

	auto GetConnectionWidget = [this, &RebuildPool](UClass* ConnectionVisualizer)
	{
		const TObjectPtr<UHeartGraphCanvasConnection>* FoundPoolWidget = RebuildPool.FindByPredicate(
			[ConnectionVisualizer](const TObjectPtr<UHeartGraphCanvasConnection>& PoolWidget)
			{
				return PoolWidget->GetClass() == ConnectionVisualizer;
			});

		if (FoundPoolWidget != nullptr && IsValid(*FoundPoolWidget))
		{
			RebuildPool.Remove(FoundPoolWidget->Get());
			return FoundPoolWidget->Get();
		}

		return CreateWidget<UHeartGraphCanvasConnection>(this, ConnectionVisualizer);
	};

	auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();
	auto&& CanvasGraphClass = GraphNode->GetGraph()->GetClass();
	auto&& CanvasGraphRegistry = RegistrySubsystem->GetRegistry(CanvasGraphClass);

	TSet<FHeartGraphPinReference> Connections = GraphNode->GetLinks(Pin).Links;
	for (const FHeartGraphPinReference& Connection : Connections)
	{
		const FHeartGraphPinDesc& ConnectionDesc = GraphNode->GetPinDesc(Connection.PinGuid);

		auto&& ConnectionVisualizer = CanvasGraphRegistry->GetVisualizerClassForGraphConnection(ThisDesc, ConnectionDesc, UWidget::StaticClass());
		if (!IsValid(ConnectionVisualizer))
		{
			continue;
		}

		if (UHeartGraphCanvasConnection* ConnectionWidget = GetConnectionWidget(ConnectionVisualizer))
		{
			ConnectionWidget->GraphCanvas = GraphCanvas;
			ConnectionWidget->FromPin = GraphNode->GetPinReference(Pin);
			ConnectionWidget->ToPin = Connection;
			ConnectionWidgets.Add(ConnectionWidget);
			GraphCanvas->AddConnectionWidget(ConnectionWidget);
		}
	}
}

UHeartGraphCanvasPin* UHeartGraphCanvasNode::GetPinWidget(const FHeartPinGuid& Pin) const
{
	for (auto&& PinWidget : PinWidgets)
	{
		if (PinWidget->GetPinGuid() == Pin)
		{
			return PinWidget;
		}
	}

	return nullptr;
}

UHeartGraphCanvasPin* UHeartGraphCanvasNode::CreatePinWidget(const FHeartPinGuid& Pin)
{
	const FHeartGraphPinDesc Desc = GraphNode->GetPinDesc(Pin);

	if (!Desc.IsValid())
	{
		return nullptr;
	}

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
			return ConnectionWidget->FromPin.PinGuid == PinWidget->GetPinGuid();
		});

	PinWidget->RemoveFromParent();
	PinWidgets.Remove(PinWidget);
}