// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasPanel.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasConnection.h"

#include "HeartCanvasConnectionVisualizer.h"
#include "ModelView/HeartGraphSchema.h"
#include "Model/HeartGraph.h"

#include "GraphRegistry/HeartNodeRegistrySubsystem.h"

#include "General/HeartMath.h"
#include "UI/HeartWidgetUtilsLibrary.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

DEFINE_LOG_CATEGORY(LogHeartGraphCanvas)

UHeartGraphCanvas::UHeartGraphCanvas()
{
	View = {0, 0, 1};
	TargetView = {0, 0, 1};
	ViewMovementScalar = {1, 1, 0.1};
	ViewBounds.Min = { -10000, -10000, 0.1 };
	ViewBounds.Max = { 10000, 10000, 10 };

	LocationModifiers = CreateDefaultSubobject<UHeartNodeLocationModifierStack>("ProxyLayerStack");
}

bool UHeartGraphCanvas::Initialize()
{
	auto Super = Super::Initialize();

	BindingContainer.SetupLinker(this);

	return Super;
}

void UHeartGraphCanvas::NativeConstruct()
{
	Super::NativeConstruct();

	//SetViewCenter(GetStartingView());
}

void UHeartGraphCanvas::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TargetView.Z != View.Z)
	{
		SetZoom(FMath::FInterpTo(View.Z, TargetView.Z, InDeltaTime, ZoomInterpSpeed));
		UpdateAllCanvasNodesZoom();
	}

	if (TargetView != View)
	{
		SetViewOffset(FMath::Vector2DInterpTo(FVector2D(View), FVector2D(TargetView), InDeltaTime, DraggingInterpSpeed));
	}

	if (NeedsToUpdatePositions)
	{
		UpdateAllPositionsOnCanvas();
		NeedsToUpdatePositions = false;
	}
}

int32 UHeartGraphCanvas::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                     const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, const int32 LayerId,
                                     const FWidgetStyle& InWidgetStyle, const bool bParentEnabled) const
{
	auto SuperLayerID = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);

	// Draw connections between pins
	if (DisplayedNodes.IsEmpty())
	{
		return SuperLayerID;
	}

	auto&& ConnectionVisualizer = GetGraph()->GetSchema()->GetConnectionVisualizer<UHeartCanvasConnectionVisualizer>();

	if (!ensure(IsValid(ConnectionVisualizer)))
	{
		UE_LOG(LogHeartGraphCanvas, Error, TEXT("ConnectionVisualizer is invalid: cannot display pin connections!"))
		return SuperLayerID;
	}

	// Get the set of pins for all children and synthesize geometry for culled out pins so lines can be drawn to them.
	TMap<UHeartGraphPin*, TPair<UHeartGraphCanvasPin*, FGeometry>> PinGeometries;
	TSet<UHeartGraphCanvasPin*> VisiblePins;

	for (auto&& DisplayedNode : DisplayedNodes)
	{
		UHeartGraphCanvasNode* GraphNode = DisplayedNode.Value;

		// If this is a culled node, approximate the pin geometry to the corner of the node it is within
		if (IsNodeCulled(GraphNode, AllottedGeometry))
		{
			auto&& PinWidgets = GraphNode->GetPinWidgets();

			const FVector2D NodeLoc = GraphNode->GetNode()->GetLocation();

			for (auto&& PinWidget : PinWidgets)
			{
				if (PinWidget->GetPin())
				{
					FVector2D PinLoc = NodeLoc; // + PinWidget->GetNodeOffset(); TODO

					const FGeometry SynthesizedPinGeometry(ScalePositionToCanvasZoom(PinLoc) * AllottedGeometry.Scale, FVector2D(AllottedGeometry.AbsolutePosition), FVector2D::ZeroVector, 1.f);
					PinGeometries.Add(PinWidget->GetPin(), {PinWidget, SynthesizedPinGeometry});
				}
			}
		}
		else
		{
			VisiblePins.Append(GraphNode->GetPinWidgets());
		}
	}

	for (auto&& VisiblePin : VisiblePins)
	{
		PinGeometries.Add(VisiblePin->GetPin(), {VisiblePin, VisiblePin->GetTickSpaceGeometry() });
	}

	/*
	// Now get the pin geometry for all visible children and append it to the PinGeometries map
	TMap<UHeartGraphCanvasPin*, TPair<UHeartGraphCanvasPin*, FGeometry>> VisiblePinGeometries;
	FindChildGeometries(AllottedGeometry, VisiblePins, VisiblePinGeometries);
	PinGeometries.Append(VisiblePinGeometries);
	*/

	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId-2, InWidgetStyle, bParentEnabled);

	// Draw preview connections (only connected on one end)
	if (PreviewConnectionPin.IsValid())
	{
		auto&& PreviewPin = ResolvePinReference(PreviewConnectionPin);

		if (IsValid(PreviewPin) && PreviewPin->GetPin())
		{
			auto&& GraphGeo = GetTickSpaceGeometry();
			FGeometry PinGeo = PinGeometries.Find(PreviewPin->GetPin())->Value;

			FVector2D StartPoint;
			FVector2D EndPoint;

			FVector CustomPosition;
			bool RelativeStart;
			const bool HandledStart = IGraphPinVisualizerInterface::Execute_GetCustomAttachmentPosition(PreviewPin, CustomPosition, RelativeStart);

			if (!HandledStart || RelativeStart)
			{
				// @todo kinda awful that this is the only way ive found to do this...
				StartPoint = GraphGeo.AbsoluteToLocal(PinGeo.LocalToAbsolute(UHeartWidgetUtilsLibrary::GetGeometryCenter(PinGeo)));
			}

			if (HandledStart)
			{
				StartPoint += FVector2D(CustomPosition);
			}

			//if (PreviewPin->GetPin()->GetDirection() == EHeartPinDirection::Input)
			{
				//StartPoint = AllottedGeometry.LocalToAbsolute(PreviewConnectorEndpoint);
				//EndPoint = PinPoint;
			}
			//else
			{
				EndPoint = GraphGeo.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
			}

			ConnectionVisualizer->PaintTimeDrawPreviewConnection(Context, StartPoint, EndPoint, PreviewPin);
		}
	}

	if (!UseDeprecatedPaintMethodToDrawConnections)
	{
		return SuperLayerID;
	}

	// Draw all regular connections
	ConnectionVisualizer->PaintTimeDrawPinConnections(Context, GetTickSpaceGeometry(), PinGeometries);
	SuperLayerID = FMath::Max(SuperLayerID, Context.MaxLayer);

	return SuperLayerID;
}

UHeartWidgetInputLinker* UHeartGraphCanvas::ResolveLinker_Implementation() const
{
	return BindingContainer.GetLinker();
}

const UHeartGraph* UHeartGraphCanvas::GetHeartGraph() const
{
	return DisplayedGraph.Get();
}

FVector2D UHeartGraphCanvas::GetNodeLocation(const FHeartNodeGuid Node) const
{
	if (ensure(IsValid(LocationModifiers)))
	{
		return LocationModifiers->ProxyToLocation(DisplayedGraph->GetNode(Node)->GetLocation());
	}

	return FVector2D();
}

void UHeartGraphCanvas::SetNodeLocation(const FHeartNodeGuid Node, const FVector2D& Location)
{
	if (ensure(IsValid(LocationModifiers)))
	{
		DisplayedGraph->GetNode(Node)->SetLocation(LocationModifiers->LocationToProxy(Location));
	}
}

void UHeartGraphCanvas::Reset()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->RemoveFromParent();
	}

	DisplayedNodes.Empty();
	SelectedNodes.Empty();
	PreviewConnectionPin = FHeartGraphPinReference();
}

void UHeartGraphCanvas::Refresh()
{
	if (!ensure(DisplayedNodes.IsEmpty()))
	{
		UE_LOG(LogHeartGraphCanvas, Warning, TEXT("HeartGraphCanvas cannot refresh, as it has not been reset."))
		return;
	}

	TArray<UHeartGraphNode*> GraphNodes;
	DisplayedGraph->GetNodeArray(GraphNodes);
	for (auto&& GraphNode : GraphNodes)
	{
		if (IsValid(GraphNode))
		{
			AddNodeToDisplay(GraphNode);
		}
	}

	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->RebuildAllPinConnections();
	}
}

void UHeartGraphCanvas::UpdateAllPositionsOnCanvas()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		UpdateNodePositionOnCanvas(DisplayedNode.Value);
	}
}

void UHeartGraphCanvas::UpdateNodePositionOnCanvas(const UHeartGraphCanvasNode* CanvasNode)
{
	auto&& Node = CanvasNode->GetNode();
	auto&& NodeLocation = Node->GetLocation();

	if (NodeLocation.ContainsNaN())
	{
		NodeLocation = FVector2D::ZeroVector;
	}

	const FVector2D Position = ScalePositionToCanvasZoom(NodeLocation);

	auto&& CanvasSlot = Cast<UCanvasPanelSlot>(CanvasNode->Slot);

	if (!ensure(IsValid(CanvasSlot)))
	{
		UE_LOG(LogHeartGraphCanvas, Error, TEXT("HeartGraphCanvasNodes must be added to a canvas"))
		return;
	}

	if (Position != CanvasSlot->GetPosition())
	{
		CanvasSlot->SetPosition(Position);
	}
}

void UHeartGraphCanvas::UpdateAllCanvasNodesZoom()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->OnZoomSet(View.Z);
	}
}

void UHeartGraphCanvas::AddNodeToDisplay(UHeartGraphNode* Node)
{
	check(Node);

	UHeartNodeRegistrySubsystem* NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>();

	auto&& VisualizerClass =
		NodeRegistrySubsystem->GetRegistry(GetGraph()->GetClass())->GetVisualizerClassForGraphNode(Node->GetClass());

	if (VisualizerClass && VisualizerClass->IsChildOf<UHeartGraphCanvasNode>())
	{
		if (auto&& Widget = CreateWidget<UHeartGraphCanvasNode>(this, VisualizerClass))
		{
			Widget->GraphCanvas = this;
			Widget->GraphNode = Node;
			DisplayedNodes.Add(Node->GetGuid(), Widget);
			NodeCanvas->AddChildToCanvas(Widget)->SetAutoSize(true);
			Widget->OnZoomSet(View.Z);
			UpdateNodePositionOnCanvas(Widget);
		}

		if (!IsDesignTime())
		{
			Node->OnNodeLocationChanged.AddDynamic(this, &ThisClass::OnNodeLocationChanged);
		}
	}
}

void UHeartGraphCanvas::SetViewOffset(const FVector2D& Value)
{
	if (Value.X != View.X || Value.Y != View.Y)
	{
		View.X = Value.X;
		View.Y = Value.Y;
		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::AddToViewOffset(const FVector2D& Value)
{
	if (!Value.IsZero())
	{
		View += Value;
		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::SetZoom(const double& Value)
{
	if (Value != View.Z)
	{
		switch (ZoomBehavior)
		{
		case EHeartGraphZoomAlgorithm::None:
			{
				View.Z = Value;
			}
			break;
		case EHeartGraphZoomAlgorithm::MouseRelative:
			{
				auto&& MouseLocation = GetTickSpaceGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

				const FVector2D PreZoomUnscaledDelta = UnscalePositionToCanvasZoom(MouseLocation);

				View.Z = Value;

				const FVector2D Adjustment = UnscalePositionToCanvasZoom(MouseLocation) - PreZoomUnscaledDelta;

				TargetView += Adjustment;
			}
			break;
		case EHeartGraphZoomAlgorithm::GraphRelative:
			{
				auto&& CanvasHalfSize = GetTickSpaceGeometry().Size * 0.5;

				const FVector2D PreZoomUnscaledDelta = UnscalePositionToCanvasZoom(CanvasHalfSize);

				View.Z = Value;

				const FVector2D Adjustment = UnscalePositionToCanvasZoom(CanvasHalfSize) - PreZoomUnscaledDelta;

				TargetView += Adjustment;
			}
			break;
		default: ;
		}

		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::AddToZoom(const double& Value)
{
	if (Value != 0.f)
	{
		SetZoom(View.Z + Value);
	}
}

void UHeartGraphCanvas::SetPreviewConnection(const FHeartGraphPinReference& Reference)
{
	PreviewConnectionPin = Reference;
}

void UHeartGraphCanvas::AddConnectionWidget(UHeartGraphCanvasConnection* ConnectionWidget)
{
	ConnectionCanvas->AddChild(ConnectionWidget);
}

bool UHeartGraphCanvas::IsNodeCulled(UHeartGraphCanvasNode* GraphNode, const FGeometry& Geometry) const
{
	static const float GuardBandArea = 0.25f;

	//if (GraphNode->ShouldAllowCulling())
	{
		const FVector2D Location = GraphNode->GetNode()->GetLocation();
		const FVector2D MinClipArea = Geometry.GetLocalSize() * -GuardBandArea;
		const FVector2D MaxClipArea = Geometry.GetLocalSize() * ( 1.f + GuardBandArea);
		const FVector2D NodeTopLeft = ScalePositionToCanvasZoom(Location);
		const FVector2D NodeBottomRight = ScalePositionToCanvasZoom(Location + GraphNode->GetDesiredSize());

		return
			NodeBottomRight.X < MinClipArea.X ||
			NodeBottomRight.Y < MinClipArea.Y ||
			NodeTopLeft.X > MaxClipArea.X ||
			NodeTopLeft.Y > MaxClipArea.Y;
	}
	//else
	//{
	//	return false;
	//}
}

void UHeartGraphCanvas::OnNodeAddedToGraph(UHeartGraphNode* Node)
{
	if (IsValid(Node))
	{
		AddNodeToDisplay(Node);
	}
}

void UHeartGraphCanvas::OnNodeRemovedFromGraph(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node)))
	{
		return;
	}

	if (DisplayedNodes.Contains(Node->GetGuid()))
	{
		if (Node->IsSelected())
		{
			UnselectNode(Node->GetGuid());
		}

		DisplayedNodes.FindAndRemoveChecked(Node->GetGuid())->RemoveFromParent();
	}

	Node->OnNodeLocationChanged.RemoveAll(this);
}

void UHeartGraphCanvas::OnNodeLocationChanged(UHeartGraphNode* Node, const FVector2D& Location)
{
	if (auto&& GraphNode = DisplayedNodes.Find(Node->GetGuid()))
	{
		UpdateNodePositionOnCanvas(*GraphNode);
	}
}

FVector2D UHeartGraphCanvas::ScalePositionToCanvasZoom(const FVector2D& Position) const
{
	return (FVector2D(View) + Position) * View.Z;
}

FVector2D UHeartGraphCanvas::UnscalePositionToCanvasZoom(const FVector2D& Position) const
{
	return (SaveDivide(Position, View.Z)) - FVector2D(View);
}

UHeartGraphCanvasPin* UHeartGraphCanvas::ResolvePinReference(const FHeartGraphPinReference& PinReference) const
{
	if (auto&& GraphNode = DisplayedNodes.Find(PinReference.NodeGuid))
	{
		if (*GraphNode)
		{
			if (auto&& Pin = (*GraphNode)->GetPinWidget(PinReference.PinGuid))
			{
				return Pin;
			}
		}
	}

	return nullptr;
}

UHeartGraphCanvasNode* UHeartGraphCanvas::GetCanvasNode(const FHeartNodeGuid NodeGuid)
{
	if (auto&& Node = DisplayedNodes.Find(NodeGuid))
	{
		return *Node;
	}
	return nullptr;
}

void UHeartGraphCanvas::SetGraph(UHeartGraph* Graph)
{
	if (DisplayedGraph.IsValid())
	{
		if (DisplayedGraph == Graph)
		{
			UE_LOG(LogHeartGraphCanvas, Warning, TEXT("Attempted to SetGraph to currently displayed graph. This will do nothing!"))
			return;
		}

		DisplayedGraph->OnNodeAdded.RemoveAll(this);
		DisplayedGraph->OnNodeRemoved.RemoveAll(this);
		Reset();
	}

	DisplayedGraph = Graph;

	if (DisplayedGraph.IsValid())
	{
		DisplayedGraph->OnNodeAdded.AddDynamic(this, &ThisClass::UHeartGraphCanvas::OnNodeAddedToGraph);
		DisplayedGraph->OnNodeRemoved.AddDynamic(this, &ThisClass::UHeartGraphCanvas::OnNodeRemovedFromGraph);
		Refresh();
	}
}

void UHeartGraphCanvas::SetViewCorner(const FVector2D& NewViewCorner, bool Interp)
{
	TargetView.X = NewViewCorner.X;
	TargetView.Y = NewViewCorner.Y;

	if (!Interp)
	{
		SetViewOffset(FVector2D(TargetView));
	}
}

void UHeartGraphCanvas::AddToViewCorner(const FVector2D& NewViewCorner, bool Interp)
{
	TargetView += NewViewCorner * FVector2D(ViewMovementScalar);

	TargetView.X = FMath::Clamp(TargetView.X, ViewBounds.Min.X, ViewBounds.Max.X);
	TargetView.Y = FMath::Clamp(TargetView.Y, ViewBounds.Min.Y, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetViewOffset(FVector2D(TargetView));
	}
}

void UHeartGraphCanvas::SetZoom(double NewZoom, bool Interp)
{
	TargetView.Z = NewZoom;

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::AddToZoom(double NewZoom, bool Interp)
{
	TargetView.Z += NewZoom * ViewMovementScalar.Z;
	TargetView.Z = FMath::Clamp(TargetView.Z, ViewBounds.Min.Z, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::SelectNode(FHeartNodeGuid NodeGuid)
{
	if (ensure(NodeGuid.IsValid()))
	{
		if (auto&& Node = DisplayedNodes.Find(NodeGuid))
		{
			SelectedNodes.Add(NodeGuid);
			(*Node)->SetNodeSelected(true);
		}
	}
}

void UHeartGraphCanvas::SelectNodes(const TArray<FHeartNodeGuid>& NodeGuids)
{
	for (auto&& NodeGuid : NodeGuids)
	{
		SelectNode(NodeGuid);
	}
}

void UHeartGraphCanvas::UnselectNode(FHeartNodeGuid NodeGuid)
{
	if (ensure(NodeGuid.IsValid()))
	{
		if (SelectedNodes.Remove(NodeGuid))
		{
			if (auto&& Node = DisplayedNodes.Find(NodeGuid))
			{
				(*Node)->SetNodeSelected(false);
			}
		}
	}
}

void UHeartGraphCanvas::ClearNodeSelection()
{
	for (auto&& SelectedGuid : SelectedNodes)
	{
		if (auto&& Node = DisplayedNodes.Find(SelectedGuid))
		{
			(*Node)->SetNodeSelected(false);
		}
	}

	SelectedNodes.Empty();
}
