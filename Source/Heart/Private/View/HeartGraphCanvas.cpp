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

UHeartGraphCanvas::UHeartGraphCanvas(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	View = {0, 0, 1};
	TargetView = {0, 0, 1};
	ViewMovementScalar = {1, 1, 0.1};
	ViewBounds.Min = { -10000, -10000, 0.1 };
	ViewBounds.Max = { 10000, 10000, 10 };
}

bool UHeartGraphCanvas::Initialize()
{
	const bool SuperInitialized = Super::Initialize();

	BindingContainer.SetLinker(NewObject<UHeartWidgetInputLinker>(this));

	return SuperInitialized;
}

void UHeartGraphCanvas::NativeConstruct()
{
	Super::NativeConstruct();

	//SetViewCenter(GetStartingView());
}

void UHeartGraphCanvas::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DraggingWithMouse)
	{
		const FVector2D MousePos = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2D();
		const FVector2D ScreenOffset = MousePos - DeltaMousePosition;

		FVector2D ViewportOffset;
		USlateBlueprintLibrary::ScreenToViewport(this, ScreenOffset, ViewportOffset);

		AddToViewCorner(ViewportOffset / View.Z, true);

		DeltaMousePosition = MousePos;
	}

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
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);


}

UHeartWidgetInputLinker* UHeartGraphCanvas::ResolveLinker() const
{
	return BindingContainer.Linker;
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
				const FVector2D PreZoomUnscaledDelta = UnscalePositionToCanvasZoom(DeltaMousePosition);

				View.Z = Value;

				auto Adjustment = UnscalePositionToCanvasZoom(DeltaMousePosition) - PreZoomUnscaledDelta;
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

void UHeartGraphCanvas::SetDragActive(bool Enabled)
{
	if (Enabled)
	{
		DraggingWithMouse = true;
		DeltaMousePosition = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2D();
	}
	else
	{
		DraggingWithMouse = false;
	}
}