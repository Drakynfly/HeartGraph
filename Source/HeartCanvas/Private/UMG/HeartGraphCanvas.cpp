﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasPanel.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"
#include "UMG/HeartGraphCanvasConnection.h"

#include "HeartCanvasConnectionVisualizer.h"
#include "ModelView/HeartGraphSchema.h"
#include "Model/HeartGraph.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "General/HeartMath.h"
#include "UI/HeartWidgetUtilsLibrary.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

DEFINE_LOG_CATEGORY(LogHeartGraphCanvas)

namespace Heart::Canvas
{
	int32 ConnectionZOrder = 0;
	int32 NodeZOrder = 1;
	int32 PopUpZOrder = 2;
}

UHeartGraphCanvas::UHeartGraphCanvas()
{
	View = {0.0, 0.0, 1.0};
	TargetView = {0.0, 0.0, 1.0};
	ViewMovementScalar = {1.0, 1.0, 0.1};
	ViewBounds.Min = { -10000.0, -10000.0, 0.1 };
	ViewBounds.Max = { 10000.0, 10000.0, 10.0 };

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

	UHeartCanvasConnectionVisualizer* ConnectionVisualizer = nullptr;

	if (DisplayedGraph.IsValid())
	{
		ConnectionVisualizer = DisplayedGraph->GetSchema()->GetConnectionVisualizer<UHeartCanvasConnectionVisualizer>();
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

			const FVector2D NodeLoc = GraphNode->GetGraphNode()->GetLocation();

			for (UHeartGraphCanvasPin* PinWidget : PinWidgets)
			{
				FHeartPinGuid WidgetGuid = PinWidget->GetPinGuid();
				if (WidgetGuid.IsValid())
				{
					FVector2D PinLoc = NodeLoc; // + PinWidget->GetNodeOffset(); TODO

					const FGeometry SynthesizedPinGeometry(ScalePositionToCanvasZoom(PinLoc) * AllottedGeometry.Scale, FVector2D(AllottedGeometry.AbsolutePosition), FVector2D::ZeroVector, 1.f);
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

UHeartGraph* UHeartGraphCanvas::GetHeartGraph() const
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
	auto&& Node = CanvasNode->GetGraphNode();
	auto&& NodeLocation = Node->GetLocation();

	if (NodeLocation.ContainsNaN())
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
		FVector2D AverageNodePosition = FVector2D::ZeroVector;

		for (FHeartNodeGuid SelectedNode : SelectedNodes)
		{
			if (auto&& Node = DisplayedNodes.Find(SelectedNode))
			{
				if (const TObjectPtr<UHeartGraphCanvasNode> CanvasNode = *Node)
				{
					// Add the canonical location of the graph node.
					AverageNodePosition += CanvasNode->GetGraphNode()->GetLocation();

					// Add half the size of the node. We are focusing on the center of the node, not the corner
					AverageNodePosition += CanvasNode->GetCachedGeometry().GetLocalSize() * 0.5 * (1.0 / View.Z);
				}
			}
		}

		AverageNodePosition *= 1.0 / SelectedNodes.Num();

		if (PanToSelectionSettings.EnablePanToSelection)
		{
			FVector2D LocalNewCorner = AverageNodePosition;

			LocalNewCorner = -LocalNewCorner;


			// Add half the size of the canvas, to "center" the focus
			LocalNewCorner += NodeCanvas->GetCachedGeometry().GetLocalSize() * 0.5;

			SetViewCorner(LocalNewCorner, true);
		}

		if (PanToSelectionSettings.EnableZoomToSelection)
		{
			float TargetZoom = 0.f;

			//SetZoom(TargetZoom, true);
		}
	}
}

void UHeartGraphCanvas::AddNodeToDisplay(UHeartGraphNode* Node)
{
	// This function is only used internally, so Node should *always* be validated prior to this point.
	check(Node);

	if (const TSubclassOf<UHeartGraphCanvasNode> VisualizerClass = GetVisualClassForNode(Node))
	{
		auto&& Widget = CreateWidget<UHeartGraphCanvasNode>(this, VisualizerClass);
		check(Widget);

		Widget->GraphCanvas = this;
		Widget->GraphNode = Node;
		Widget->PostInitNode();
		DisplayedNodes.Add(Node->GetGuid(), Widget);

		UCanvasPanelSlot* NodeSlot = NodeCanvas->AddChildToCanvas(Widget);
		NodeSlot->SetZOrder(Heart::Canvas::NodeZOrder);
		NodeSlot->SetAutoSize(true);

		Widget->OnZoomSet(View.Z);
		UpdateNodePositionOnCanvas(Widget);

		if (!IsDesignTime())
		{
			Node->OnNodeLocationChanged.AddDynamic(this, &ThisClass::OnNodeLocationChanged);
		}
	}
	else
	{
		UE_LOG(LogHeartGraphCanvas, Warning, TEXT("Unable to determine Visual Class. Node '%s' will not be displayed"), *Node->GetName())
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

	return RegistrySubsystem->GetRegistry(DisplayedGraph->GetClass())
								->GetVisualizerClassForGraphNode<UHeartGraphCanvasNode>(Node->GetClass());
}

void UHeartGraphCanvas::SetPreviewConnection(const FHeartGraphPinReference& Reference)
{
	PreviewConnectionPin = Reference;
}

UCanvasPanelSlot* UHeartGraphCanvas::AddConnectionWidget(UHeartGraphCanvasConnection* ConnectionWidget)
{
	UCanvasPanelSlot* ConnectionSlot = NodeCanvas->AddChildToCanvas(ConnectionWidget);
	ConnectionSlot->SetZOrder(Heart::Canvas::ConnectionZOrder);
	return ConnectionSlot;
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
		const FVector2D Location = GraphNode->GetGraphNode()->GetLocation();
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
	return SafeDivide(Position, View.Z) - FVector2D(View);
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

		DisplayedGraph->GetOnNodeAdded().RemoveAll(this);
		DisplayedGraph->GetOnNodeRemoved().RemoveAll(this);
		Reset();
	}

	DisplayedGraph = Graph;

	if (DisplayedGraph.IsValid())
	{
		DisplayedGraph->GetOnNodeAdded().AddUObject(this, &UHeartGraphCanvas::OnNodeAddedToGraph);
		DisplayedGraph->GetOnNodeRemoved().AddUObject(this, &UHeartGraphCanvas::OnNodeRemovedFromGraph);
		Refresh();
	}
}

void UHeartGraphCanvas::SetViewCorner(const FVector2D& NewViewCorner, const bool Interp)
{
	TargetView.X = NewViewCorner.X;
	TargetView.Y = NewViewCorner.Y;

	if (!Interp)
	{
		SetViewOffset(FVector2D(TargetView));
	}
}

void UHeartGraphCanvas::AddToViewCorner(const FVector2D& NewViewCorner, const bool Interp)
{
	TargetView += NewViewCorner * FVector2D(ViewMovementScalar);

	TargetView.X = FMath::Clamp(TargetView.X, ViewBounds.Min.X, ViewBounds.Max.X);
	TargetView.Y = FMath::Clamp(TargetView.Y, ViewBounds.Min.Y, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetViewOffset(FVector2D(TargetView));
	}
}

void UHeartGraphCanvas::SetZoom(const double NewZoom, const bool Interp)
{
	TargetView.Z = NewZoom;

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::AddToZoom(const double NewZoom, const bool Interp)
{
	TargetView.Z += NewZoom * ViewMovementScalar.Z;
	TargetView.Z = FMath::Clamp(TargetView.Z, ViewBounds.Min.Z, ViewBounds.Max.Y);

	if (!Interp)
	{
		SetZoom(TargetView.Z);
	}
}

void UHeartGraphCanvas::SelectNode(const FHeartNodeGuid Node)
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

void UHeartGraphCanvas::UnselectNode(const FHeartNodeGuid Node)
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

bool UHeartGraphCanvas::IsNodeSelected(const FHeartNodeGuid Node) const
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
