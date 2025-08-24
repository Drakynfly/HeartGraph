// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasPanel.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasConnection.h"

#include "HeartCanvasPrivate.h"

#include "HeartCanvasConnectionVisualizer.h"
#include "HeartWidgetUtilsLibrary.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartLayoutHelper.h"

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "General/HeartMath.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvas)

DECLARE_CYCLE_STAT(TEXT("CanvasTick"), STAT_CanvasTick, STATGROUP_HeartCanvas);
DECLARE_CYCLE_STAT(TEXT("CanvasPaint"), STAT_CanvasPaint, STATGROUP_HeartCanvas);

DEFINE_LOG_CATEGORY(LogHeartGraphCanvas)

namespace Heart::Canvas
{
	int32 ConnectionZOrder = 0;
	int32 NodeZOrder = 1;
	int32 PopUpZOrder = 2;
}

UHeartGraphCanvas::UHeartGraphCanvas()
{
	View = {0.f, 0.f, 1.f};
	TargetView = {0.f, 0.f, 1.f};
	ViewMovementScalar = {1.f, 1.f, 0.1f};
	ViewBounds.Min = { -10000.f, -10000.f, 0.1f };
	ViewBounds.Max = { 10000.f, 10000.f, 10.f };

	LocationModifiers = CreateDefaultSubobject<UHeartNodeLocationModifierStack>("LocationModifiers");
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

	Refresh();
}

void UHeartGraphCanvas::NativeDestruct()
{
	Reset();

	Super::NativeDestruct();
}

void UHeartGraphCanvas::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_CanvasTick)

	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TargetView.Z != View.Z)
	{
		SetZoom(FMath::FInterpTo(View.Z, TargetView.Z, InDeltaTime, ZoomInterpSpeed));
		UpdateAllCanvasNodesZoom();
	}

	if (TargetView != View)
	{
		SetViewOffset(Vector2fInterpTo(FVector2f(View), FVector2f(TargetView), InDeltaTime, DraggingInterpSpeed));
	}

	if (RunLayoutOnTick && IsValid(Layout))
	{
		Layout->Layout(this, InDeltaTime);
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
	SCOPE_CYCLE_COUNTER(STAT_CanvasPaint)

	auto SuperLayerID = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);

	if (!UseDeprecatedPaintMethodToDrawConnections)
	{
		return SuperLayerID;
	}

	// Draw connections between pins
	if (DisplayedNodes.IsEmpty())
	{
		return SuperLayerID;
	}

	UHeartCanvasConnectionVisualizer* ConnectionVisualizer = nullptr;

	if (DisplayedGraph.IsValid())
	{
		//ConnectionVisualizer = DisplayedGraph->GetSchema()->GetConnectionVisualizer<UHeartCanvasConnectionVisualizer>();
	}

	if (!ensure(IsValid(ConnectionVisualizer)))
	{
		UE_LOG(LogHeartGraphCanvas, Error, TEXT("ConnectionVisualizer is invalid: cannot display pin connections!"))
		return SuperLayerID;
	}

	// Get the set of pins for all children and synthesize geometry for culled out pins so lines can be drawn to them.
	TMap<FHeartPinGuid, TPair<UHeartGraphCanvasPin*, FGeometry>> PinGeometries;
	TSet<UHeartGraphCanvasPin*> VisiblePins;

	for (auto&& DisplayedNode : DisplayedNodes)
	{
		UHeartGraphCanvasNode* GraphNode = DisplayedNode.Value;

		// If this is a culled node, approximate the pin geometry to the corner of the node it is within
		if (IsNodeCulled(GraphNode, AllottedGeometry))
		{
			auto&& PinWidgets = GraphNode->GetPinWidgets();

			const FVector2f NodeLoc(GraphNode->GetGraphNode()->GetLocation());

			for (UHeartGraphCanvasPin* PinWidget : PinWidgets)
			{
				FHeartPinGuid WidgetGuid = PinWidget->GetPinGuid();
				if (WidgetGuid.IsValid())
				{
					FVector2f PinLoc = NodeLoc; // + PinWidget->GetNodeOffset(); TODO

					const FGeometry SynthesizedPinGeometry(ScalePositionToCanvasZoom_2f(PinLoc) * AllottedGeometry.Scale, AllottedGeometry.AbsolutePosition, FVector2f::ZeroVector, 1.f);
					PinGeometries.Add(WidgetGuid, {PinWidget, SynthesizedPinGeometry});
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
		PinGeometries.Add(VisiblePin->GetPinGuid(), {VisiblePin, VisiblePin->GetTickSpaceGeometry() });
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

		if (IsValid(PreviewPin))
		{
			auto&& GraphGeo = GetTickSpaceGeometry();
			FGeometry PinGeo = PinGeometries.Find(PreviewPin->GetPinGuid())->Value;

			FVector2f StartPoint;
			FVector2f EndPoint;

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
				StartPoint += CustomPosition;
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

	// Draw all regular connections
	ConnectionVisualizer->PaintTimeDrawPinConnections(Context, GetTickSpaceGeometry(), PinGeometries);
	SuperLayerID = FMath::Max(SuperLayerID, Context.MaxLayer);

	return SuperLayerID;
}

UHeartInputLinkerBase* UHeartGraphCanvas::ResolveLinker_Implementation() const
{
	return BindingContainer.GetLinker();
}

UHeartGraph* UHeartGraphCanvas::GetHeartGraph() const
{
	return DisplayedGraph.Get();
}

FVector2D UHeartGraphCanvas::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	if (ensure(IsValid(LocationModifiers)))
	{
		FVector2D Location;

		if (SyncNodeLocationsWithGraph || !NodeLocations.Contains(Node))
		{
			Location = DisplayedGraph->GetNode(Node)->GetLocation();
		}
		else
		{
			Location = NodeLocations[Node];
		}

		return LocationModifiers->ProxyToLocation(Location);
	}

	return FVector2D();
}

void UHeartGraphCanvas::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, const bool InProgressMove)
{
	if (ensure(IsValid(LocationModifiers)))
	{
		const FVector2D ProxiedLocation = LocationModifiers->LocationToProxy(Location);

		if (SyncNodeLocationsWithGraph)
		{
			UHeartGraphNode* GraphNode = DisplayedGraph->GetNode(Node);
			GraphNode->SetLocation(ProxiedLocation);
		}
		else
		{
			NodeLocations.FindOrAdd(Node) = ProxiedLocation;
			InvalidateNodeDisplay(Node, EHeartGraphCanvasInvalidateType::NodeLocation);
		}
	}
}

void UHeartGraphCanvas::Reset()
{
	ClearPopups();

	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->GraphNode->GetOnNodeLocationChanged().RemoveAll(this);
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

	if (!DisplayedGraph.IsValid())
	{
		return;
	}

	TArray<UHeartGraphNode*> GraphNodes;
	DisplayedGraph->GetNodeArray(GraphNodes);
	DisplayedNodes.Reserve(GraphNodes.Num());
	for (auto&& GraphNode : GraphNodes)
	{
		if (IsValid(GraphNode))
		{
			AddNodeToDisplay(GraphNode, false);
		}
	}

	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->PostInitNode();
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
	auto&& Node = CanvasNode->GetGraphNode();

	FVector2D NodeLocation;

	if (!SyncNodeLocationsWithGraph && NodeLocations.Contains(Node->GetGuid()))
	{
		NodeLocation = NodeLocations[Node->GetGuid()];
	}
	else
	{
		NodeLocation = Node->GetLocation();
	}

	if (UNLIKELY(NodeLocation.ContainsNaN()))
	{
		NodeLocation = FVector2D::ZeroVector;
	}

	auto&& CanvasSlot = Cast<UCanvasPanelSlot>(CanvasNode->Slot);

	if (!ensure(IsValid(CanvasSlot)))
	{
		UE_LOG(LogHeartGraphCanvas, Error, TEXT("HeartGraphCanvasNodes must be added to a canvas"))
		return;
	}

	const FVector2D Position = ScalePositionToCanvasZoom(NodeLocation);
	//const FVector2D ProxiedPosition = LocationModifiers->LocationToProxy(Position);

	CanvasSlot->SetPosition(Position);
}

void UHeartGraphCanvas::UpdateAllCanvasNodesZoom()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->OnZoomSet(View.Z);
	}
}

void UHeartGraphCanvas::UpdateAfterSelectionChanged()
{
	if ((PanToSelectionSettings.EnablePanToSelection ||
		PanToSelectionSettings.EnableZoomToSelection) &&
		!SelectedNodes.IsEmpty())
	{
		FVector2f AverageNodePosition = FVector2f::ZeroVector;

		for (const FHeartNodeGuid& SelectedNode : SelectedNodes)
		{
			if (auto&& Node = DisplayedNodes.Find(SelectedNode))
			{
				if (const TObjectPtr<UHeartGraphCanvasNode> CanvasNode = *Node)
				{
					// Add the canonical location of the graph node.
					AverageNodePosition += FVector2f(CanvasNode->GetGraphNode()->GetLocation());

					// Add half the size of the node. We are focusing on the center of the node, not the corner
					AverageNodePosition += CanvasNode->GetCachedGeometry().GetLocalSize() * 0.5f * (1.0f / static_cast<float>(View.Z));
				}
			}
		}

		AverageNodePosition *= 1.f / SelectedNodes.Num();

		if (PanToSelectionSettings.EnablePanToSelection)
		{
			FVector2f LocalNewCorner = AverageNodePosition;

			LocalNewCorner = -LocalNewCorner;


			// Add half the size of the canvas, to "center" the focus
			LocalNewCorner += NodeCanvas->GetCachedGeometry().GetLocalSize() * 0.5f;

			SetViewCorner(FVector2D(LocalNewCorner), true);
		}

		if (PanToSelectionSettings.EnableZoomToSelection)
		{
			float TargetZoom = 0.f;

			// @todo implement
			//SetZoom(TargetZoom, true);
		}
	}
}

void UHeartGraphCanvas::CreatePreviewConnection()
{
	auto&& ConnectionVisualizer = GetVisualClassForPreviewConnection();
	if (!IsValid(ConnectionVisualizer))
	{
		return;
	}

	PreviewConnection = CreateWidget<UHeartGraphCanvasConnection>(this, ConnectionVisualizer);
	if (IsValid(PreviewConnection))
	{
		PreviewConnection->GraphCanvas = this;
		PreviewConnection->FromPin = PreviewConnectionPin;

		AddConnectionWidget(PreviewConnection);
	}
}

void UHeartGraphCanvas::AddNodeToDisplay(UHeartGraphNode* Node, const bool InitNodeWidget)
{
	// This function is only used internally, so Node should *always* be validated prior to this point.
	check(Node);

	if (const TSubclassOf<UHeartGraphCanvasNode> VisualizerClass = GetVisualClassForNode(Node))
	{
		auto&& Widget = CreateWidget<UHeartGraphCanvasNode>(this, VisualizerClass);
		check(Widget);

		Widget->GraphCanvas = this;
		Widget->GraphNode = Node;
		DisplayedNodes.Add(Node->GetGuid(), Widget);

		UCanvasPanelSlot* NodeSlot = NodeCanvas->AddChildToCanvas(Widget);
		NodeSlot->SetZOrder(Heart::Canvas::NodeZOrder);
		NodeSlot->SetAutoSize(true);

		Widget->OnZoomSet(View.Z);
		UpdateNodePositionOnCanvas(Widget);

		if (InitNodeWidget)
		{
			Widget->PostInitNode();
		}

		if (!IsDesignTime() && SyncNodeLocationsWithGraph)
		{
			Node->GetOnNodeLocationChanged().AddUObject(this, &ThisClass::OnNodeLocationChanged);
		}
	}
	else
	{
		UE_LOG(LogHeartGraphCanvas, Warning, TEXT("Unable to determine Visual Class. Node '%s' will not be displayed"), *Node->GetName())
	}
}

void UHeartGraphCanvas::SetViewOffset(const FVector2f& Value)
{
	if (Value.X != View.X || Value.Y != View.Y)
	{
		View.X = Value.X;
		View.Y = Value.Y;
		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::AddToViewOffset(const FVector2f& Value)
{
	if (!Value.IsZero())
	{
		View += Value;
		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::SetZoom(const float Value)
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
				const FVector2f MouseLocation = GetTickSpaceGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

				const FVector2f PreZoomUnscaledDelta = UnscalePositionToCanvasZoom_2f(MouseLocation);

				View.Z = Value;

				const FVector2f Adjustment = UnscalePositionToCanvasZoom_2f(MouseLocation) - PreZoomUnscaledDelta;

				TargetView += Adjustment;
			}
			break;
		case EHeartGraphZoomAlgorithm::GraphRelative:
			{
				const FVector2f CanvasHalfSize = GetTickSpaceGeometry().Size * 0.5f;

				const FVector2f PreZoomUnscaledDelta = UnscalePositionToCanvasZoom_2f(CanvasHalfSize);

				View.Z = Value;

				const FVector2f Adjustment = UnscalePositionToCanvasZoom_2f(CanvasHalfSize) - PreZoomUnscaledDelta;

				TargetView += Adjustment;
			}
			break;
		default: ;
		}

		NeedsToUpdatePositions = true;
		OnGraphViewChanged.Broadcast();
	}
}

void UHeartGraphCanvas::AddToZoom(const float Value)
{
	if (Value != 0.f)
	{
		SetZoom(View.Z + Value);
	}
}

void UHeartGraphCanvas::SetPreviewConnection(const FHeartGraphPinReference& Reference)
{
	if (PreviewConnectionPin != Reference)
	{
		if (PreviewConnection)
		{
			// Destroy existing connection widget
			PreviewConnection->RemoveFromParent();
			PreviewConnection = nullptr;
		}

		PreviewConnectionPin = Reference;
		if (PreviewConnectionPin.IsValid())
		{
			CreatePreviewConnection();
		}
	}
}

UCanvasPanelSlot* UHeartGraphCanvas::AddConnectionWidget(UHeartGraphCanvasConnection* ConnectionWidget)
{
	UCanvasPanelSlot* ConnectionSlot = NodeCanvas->AddChildToCanvas(ConnectionWidget);
	ConnectionSlot->SetZOrder(Heart::Canvas::ConnectionZOrder);
	return ConnectionSlot;
}

TSubclassOf<UHeartGraphCanvasNode> UHeartGraphCanvas::GetVisualClassForNode_Implementation(const UHeartGraphNode* Node) const
{
	auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	if (!IsValid(RegistrySubsystem))
	{
		UE_LOG(LogHeartGraphCanvas, Error,
			TEXT("Registry Subsystem not found! Make sure to enable `CreateRuntimeRegistrySubsystem` in project settings to access the subsystem!\n"
					"This error occured in UHeartGraphCanvas::GetVisualClassForNode. You can override this function to not use the registry subsystem if `CreateRuntimeRegistrySubsystem` is disabled on purpose!"))
		return nullptr;
	}

	auto&& CanvasGraphRegistry = RegistrySubsystem->GetNodeRegistryForGraph(DisplayedGraph.Get());
	if (!IsValid(CanvasGraphRegistry))
	{
		return nullptr;
	}

	return CanvasGraphRegistry->GetVisualizerClassForGraphNode<UHeartGraphCanvasNode>(Node->GetClass());
}

TSubclassOf<UHeartGraphCanvasConnection> UHeartGraphCanvas::GetVisualClassForConnection_Implementation(
	const FHeartGraphPinDesc& FromDesc, const FHeartGraphPinDesc& ToDesc) const
{
	auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	if (!IsValid(RegistrySubsystem))
	{
		UE_LOG(LogHeartGraphCanvas, Error,
			TEXT("Registry Subsystem not found! Make sure to enable `CreateRuntimeRegistrySubsystem` in project settings to access the subsystem!\n"
					"This error occured in UHeartGraphCanvas::GetVisualClassForPreviewConnection. You can override this function to not use the registry subsystem if `CreateRuntimeRegistrySubsystem` is disabled on purpose!"))
		return nullptr;
	}

	auto&& CanvasGraphRegistry = RegistrySubsystem->GetNodeRegistryForGraph(DisplayedGraph.Get());
	if (!IsValid(CanvasGraphRegistry))
	{
		return nullptr;
	}

	return CanvasGraphRegistry->GetVisualizerClassForGraphConnection<UHeartGraphCanvasConnection>(FromDesc, ToDesc);
}

TSubclassOf<UHeartGraphCanvasConnection> UHeartGraphCanvas::GetVisualClassForPreviewConnection_Implementation() const
{
	auto&& PreviewNode = DisplayedGraph->GetNode(PreviewConnectionPin.NodeGuid);
	if (!IsValid(PreviewNode))
	{
		return nullptr;
	}

	auto&& PinView = PreviewNode->ViewPin(PreviewConnectionPin.PinGuid);
	if (!PinView.IsValid())
	{
		return nullptr;
	}

	return GetVisualClassForConnection(PinView.Get(), Heart::Graph::InvalidPinDesc);
}

bool UHeartGraphCanvas::IsNodeCulled(const UHeartGraphCanvasNode* GraphNode, const FGeometry& Geometry) const
{
	static const float GuardBandArea = 0.25f;

	if (!IsValid(GraphNode) || !IsValid(GraphNode->GetGraphNode()))
	{
		return true;
	}

	//if (GraphNode->ShouldAllowCulling())
	{
		const FVector2f Location(GraphNode->GetGraphNode()->GetLocation());
		const FVector2f MinClipArea = Geometry.GetLocalSize() * -GuardBandArea;
		const FVector2f MaxClipArea = Geometry.GetLocalSize() * ( 1.f + GuardBandArea);
		const FVector2f NodeTopLeft = ScalePositionToCanvasZoom_2f(Location);
		const FVector2f NodeBottomRight = ScalePositionToCanvasZoom_2f(Location + FVector2f(GraphNode->GetDesiredSize()));

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
		AddNodeToDisplay(Node, true);
	}
}

void UHeartGraphCanvas::OnNodeRemovedFromGraph(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node)))
	{
		return;
	}

	Node->GetOnNodeLocationChanged().RemoveAll(this);

	auto&& NodeGuid = Node->GetGuid();

	if (SelectedNodes.Contains(NodeGuid))
	{
		UnselectNode(NodeGuid);
	}

	if (DisplayedNodes.Contains(NodeGuid))
	{
		auto&& Value = DisplayedNodes.FindAndRemoveChecked(NodeGuid);

		if (Value->GraphNode.IsValid())
		{
			Value->GraphNode->GetOnNodeLocationChanged().RemoveAll(this);
		}

		Value->RemoveFromParent();
	}
}

void UHeartGraphCanvas::OnNodeLocationChanged(UHeartGraphNode* Node, const FVector2D& Location)
{
	if (auto&& GraphNode = DisplayedNodes.Find(Node->GetGuid()))
	{
		UpdateNodePositionOnCanvas(*GraphNode);
	}
}

FVector2f UHeartGraphCanvas::ScalePositionToCanvasZoom_2f(const FVector2f& Position) const
{
	return (FVector2f(View) + Position) * View.Z;
}

FVector2f UHeartGraphCanvas::UnscalePositionToCanvasZoom_2f(const FVector2f& Position) const
{
	return SafeDivide(Position, View.Z) - FVector2f(View);
}

FVector2D UHeartGraphCanvas::ScalePositionToCanvasZoom(const FVector2D& Position) const
{
	return FVector2D(ScalePositionToCanvasZoom_2f(FVector2f(Position)));
}

FVector2D UHeartGraphCanvas::UnscalePositionToCanvasZoom(const FVector2D& Position) const
{
	return FVector2D(UnscalePositionToCanvasZoom_2f(FVector2f(Position)));
}

void UHeartGraphCanvas::InvalidateNodeDisplay(const FHeartNodeGuid& NodeGuid, const EHeartGraphCanvasInvalidateType Type)
{
	switch (Type)
	{
	case EHeartGraphCanvasInvalidateType::Full:
		{
			const bool Selected = SelectedNodes.Contains(NodeGuid);

			UHeartGraphNode* GraphNode = GetGraph()->GetNode(NodeGuid);

			OnNodeRemovedFromGraph(GraphNode);
			OnNodeAddedToGraph(GraphNode);

			if (Selected && IsValid(GraphNode))
			{
				SelectNode(NodeGuid);
			}
		}
		break;
	case EHeartGraphCanvasInvalidateType::NodeLocation:
		if (auto&& CanvasNode = GetCanvasNode(NodeGuid))
		{
			UpdateNodePositionOnCanvas(CanvasNode);
		}
		break;
	case EHeartGraphCanvasInvalidateType::Connections:
		if (auto&& CanvasNode = GetCanvasNode(NodeGuid))
		{
			CanvasNode->RebuildAllPinConnections();
		}
		break;
	}
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

UHeartGraphCanvasNode* UHeartGraphCanvas::GetCanvasNode(const FHeartNodeGuid& NodeGuid)
{
	if (auto&& Node = DisplayedNodes.Find(NodeGuid))
	{
		return *Node;
	}
	return nullptr;
}

void UHeartGraphCanvas::AddToViewCorner(const FVector2f& NewViewCorner, const bool Interp)
{
	const FVector2f Adjustment(NewViewCorner * FVector2f(ViewMovementScalar));

	TargetView.X += FMath::Clamp(Adjustment.X, ViewBounds.Min.X, ViewBounds.Max.X);
	TargetView.Y += FMath::Clamp(Adjustment.Y, ViewBounds.Min.Y, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetViewOffset(FVector2f(TargetView));
	}
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

		DisplayedGraph->GetOnNodeAdded().RemoveAll(this);
		DisplayedGraph->GetOnNodeRemoved().RemoveAll(this);
		Reset();
	}

	DisplayedGraph = Graph;

	if (DisplayedGraph.IsValid())
	{
		DisplayedGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAddedToGraph);
		DisplayedGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemovedFromGraph);
		Refresh();
	}
}

void UHeartGraphCanvas::SetViewCorner(const FVector2D& NewViewCorner, const bool Interp)
{
	TargetView.X = static_cast<float>(NewViewCorner.X);
	TargetView.Y = static_cast<float>(NewViewCorner.Y);

	if (!Interp)
	{
		SetViewOffset(FVector2f(TargetView));
	}
}

void UHeartGraphCanvas::AddToViewCorner(const FVector2D& NewViewCorner, const bool Interp)
{
	AddToViewCorner(FVector2f(NewViewCorner), Interp);
}

void UHeartGraphCanvas::SetZoom(const float NewZoom, const bool Interp)
{
	TargetView.Z = NewZoom;

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::AddToZoom(const float NewZoom, const bool Interp)
{
	TargetView.Z += NewZoom * ViewMovementScalar.Z;
	TargetView.Z = FMath::Clamp(TargetView.Z, ViewBounds.Min.Z, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::SelectNode(const FHeartNodeGuid& Node)
{
	if (ensure(Node.IsValid()))
	{
		bool IsAlreadyInSetPtr;
		SelectedNodes.Add(Node, &IsAlreadyInSetPtr);
		if (!IsAlreadyInSetPtr)
		{
			if (auto&& NodePtr = DisplayedNodes.Find(Node))
			{
				(*NodePtr)->SetNodeSelectedFromGraph(true);

				UpdateAfterSelectionChanged();
			}
		}
	}
}

void UHeartGraphCanvas::SelectNodes(const TArray<FHeartNodeGuid>& Nodes)
{
	for (auto&& NodeGuid : Nodes)
	{
		SelectNode(NodeGuid);
	}
}

void UHeartGraphCanvas::UnselectNode(const FHeartNodeGuid& Node)
{
	if (ensure(Node.IsValid()))
	{
		if (SelectedNodes.Remove(Node))
		{
			if (auto&& NodePtr = DisplayedNodes.Find(Node))
			{
				(*NodePtr)->SetNodeSelectedFromGraph(false);

				UpdateAfterSelectionChanged();
			}
		}
	}
}

bool UHeartGraphCanvas::IsNodeSelected(const FHeartNodeGuid& Node) const
{
	return SelectedNodes.Contains(Node);
}

void UHeartGraphCanvas::ClearNodeSelection()
{
	for (auto&& SelectedGuid : SelectedNodes)
	{
		if (auto&& Node = DisplayedNodes.Find(SelectedGuid))
		{
			(*Node)->SetNodeSelectedFromGraph(false);
		}
	}

	SelectedNodes.Empty();
}

UCanvasPanelSlot* UHeartGraphCanvas::AddWidgetToPopups(UWidget* Widget, const FVector2D Location)
{
	if (!IsValid(Widget)) return nullptr;

	Popups.Add(Widget);

	UCanvasPanelSlot* CanvasSlot = NodeCanvas->AddChildToCanvas(Widget);
	CanvasSlot->SetPosition(Location);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetZOrder(Heart::Canvas::PopUpZOrder);
	return CanvasSlot;
}

bool UHeartGraphCanvas::RemoveWidgetFromPopups(UWidget* Widget)
{
	if (!IsValid(Widget)) return false;

	Popups.Remove(Widget);

	return NodeCanvas->RemoveChild(Widget);
}

void UHeartGraphCanvas::ClearPopups()
{
	TArray<TObjectPtr<UWidget>> PopupsCopy = Popups;

	for (auto&& Widget : PopupsCopy)
	{
		RemoveWidgetFromPopups(Widget);
	}

	Popups.Empty();
}