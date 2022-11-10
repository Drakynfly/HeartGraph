// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/HeartGraphCanvas.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "View/HeartGraphCanvasNode.h"
#include "Model/HeartGraph.h"

#include "General/HeartMath.h"

#include "Components/CanvasPanelSlot.h"
#include "Model/HeartNodeRegistrySubsystem.h"
#include "ModelView/HeartGraphSchema.h"
#include "UI/HeartWidgetUtilsLibrary.h"
#include "View/HeartCanvasConnectionVisualizer.h"
#include "View/HeartGraphCanvasPin.h"

UHeartGraphCanvas::UHeartGraphCanvas(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	View = {0, 0, 1};
	TargetView = {0, 0, 1};
	ViewMovementScalar = {1, 1, 0.1};
	ViewBounds.Min = { -10000, -10000, 0.1 };
	ViewBounds.Max = { 10000, 10000, 10 };
}

void UHeartGraphCanvas::PostInitProperties()
{
	Super::PostInitProperties();

	BindingContainer.SetupLinker(this);
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
	if (!DisplayedNodes.IsEmpty())
	{
		auto&& ConnectionVisualizer = GetGraph()->GetSchema()->GetConnectionVisualizer();

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

		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		// Draw preview connections (only connected on one end)
		if (PreviewConnectionPin.IsValid())
		{
			auto&& PreviewPin = ResolvePinReference(PreviewConnectionPin);

			if (IsValid(PreviewPin) && PreviewPin->GetPin())
			{
				FGeometry PinGeo = PinGeometries.Find(PreviewPin->GetPin())->Value;

				FVector2D StartPoint;
				FVector2D EndPoint;

				const FVector2D PinPoint = GetTickSpaceGeometry().AbsoluteToLocal(PinGeo.LocalToAbsolute(UHeartWidgetUtilsLibrary::GetGeometryCenter(PinGeo)));

				//if (PreviewPin->GetPin()->GetDirection() == EHeartPinDirection::Input)
				{
					//StartPoint = AllottedGeometry.LocalToAbsolute(PreviewConnectorEndpoint);
					//EndPoint = PinPoint;
				}
				//else
				{
					StartPoint = PinPoint;
					EndPoint = GetTickSpaceGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
				}

				ConnectionVisualizer->PaintTimeDrawPreviewConnection(Context, StartPoint, EndPoint, PreviewPin);
			}
		}

		// Draw all regular connections
		ConnectionVisualizer->PaintTimeDrawPinConnections(Context, GetTickSpaceGeometry(), PinGeometries);
		SuperLayerID = FMath::Max(SuperLayerID, Context.MaxLayer);
	}

	return SuperLayerID;
}

UHeartWidgetInputLinker* UHeartGraphCanvas::ResolveLinker() const
{
	return BindingContainer.GetLinker();
}

void UHeartGraphCanvas::Reset()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		DisplayedNode.Value->RemoveFromParent();
	}

	DisplayedNodes.Empty();
}

void UHeartGraphCanvas::Refresh()
{
	if (!ensure(DisplayedNodes.IsEmpty()))
	{
		UE_LOG(LogTemp, Warning, TEXT("HeartGraphCanvas cannot refresh, as it has not been reset."))
		return;
	}

	TArray<UHeartGraphNode*> GraphNodes;
	DisplayedGraph->GetNodeArray(GraphNodes);
	for (auto&& GraphNode : GraphNodes)
	{
		AddNodeToDisplay(GraphNode);
	}
}

void UHeartGraphCanvas::UpdateAllPositionsOnCanvas()
{
	for (auto&& DisplayedNode : DisplayedNodes)
	{
		UpdateNodePositionOnCanvas(DisplayedNode.Value);
	}
}

void UHeartGraphCanvas::UpdateNodePositionOnCanvas(UHeartGraphCanvasNode* CanvasNode)
{
	auto&& Node = CanvasNode->GetNode();

	auto NodeLocation = Node->GetLocation();

	if (NodeLocation.ContainsNaN())
	{
		NodeLocation = FVector2D::ZeroVector;
	}

	const FVector2D Position = ScalePositionToCanvasZoom(NodeLocation);

	auto&& CanvasSlot = Cast<UCanvasPanelSlot>(CanvasNode->Slot);

	if (!ensure(IsValid(CanvasSlot)))
	{
		UE_LOG(LogTemp, Error, TEXT("HeartGraphCanvasNodes must be added to a canvas"))
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

	UHeartNodeRegistrySubsystem* NodeRegistrySubsystem = GetGameInstance()->GetSubsystem<UHeartNodeRegistrySubsystem>();

	auto&& VisualizerClass =
		NodeRegistrySubsystem->GetRegistry(GetGraph()->GetClass())->GetVisualizerClassForGraphNode(Node->GetClass());

	if (VisualizerClass->IsChildOf<UHeartGraphCanvasNode>())
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
				const FVector2D PreZoomUnscaledDelta = UnscalePositionToCanvasZoom(FSlateApplication::Get().GetCursorPos());

				View.Z = Value;

				auto Adjustment = UnscalePositionToCanvasZoom(FSlateApplication::Get().GetCursorPos()) - PreZoomUnscaledDelta;
				TargetView += Adjustment;

				NeedsToUpdatePositions = true;
				OnGraphViewChanged.Broadcast();
			}
			break;
		case EHeartGraphZoomAlgorithm::GraphRelative:
			{
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
	AddNodeToDisplay(Node);
}

void UHeartGraphCanvas::OnNodeRemovedFromGraph(UHeartGraphNode* Node)
{
	if (DisplayedNodes.Contains(Node->GetGuid()))
	{
		DisplayedNodes.FindAndRemoveChecked(Node->GetGuid())->RemoveFromParent();
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

void UHeartGraphCanvas::SetGraph(UHeartGraph* Graph)
{
	if (DisplayedGraph)
	{
		DisplayedGraph->OnNodeAdded.RemoveAll(this);
		DisplayedGraph->OnNodeRemoved.RemoveAll(this);
		Reset();
	}

	DisplayedGraph = Graph;

	if (DisplayedGraph)
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