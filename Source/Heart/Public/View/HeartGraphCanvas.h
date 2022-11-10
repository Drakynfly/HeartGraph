// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "General/VectorBounds.h"
#include "Model/HeartGraphPinReference.h"
#include "ModelView/HeartGraphNode.h"
#include "UI/HeartWidgetInputLinkerRedirector.h"
#include "HeartGraphCanvas.generated.h"

class UCanvasPanel;
class UHeartGraph;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

UENUM()
enum class EHeartGraphZoomAlgorithm : uint8
{
	None,			// No compensation to view will be applied when zooming
	MouseRelative,	// Adjust the view to maintain mouse position
	GraphRelative,	// Adjust the view to maintain graph position
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGraphViewChanged);

/**
 * Base class for displaying a graph using UMG widgets on a canvas panel.
 */
UCLASS(Abstract)
class HEART_API UHeartGraphCanvas : public UHeartGraphWidgetBase, public IHeartWidgetInputLinkerRedirector
{
	GENERATED_BODY()

public:
	UHeartGraphCanvas(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** UUserWidget */
	virtual void PostInitProperties() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	/** UUserWidget */

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker() const override;
	/** IHeartWidgetInputLinkerRedirector */

	// Used by UHeartPinConnectionDragDropOperation to notify us about what its doing so we can draw the preview link
	void SetPreviewConnection(const FHeartGraphPinReference& Reference);

protected:
	bool IsNodeCulled(UHeartGraphCanvasNode* GraphNode, const FGeometry& Geometry) const;

	void Reset();

	void Refresh();

	/** Update the position of every node on the canvas. */
	void UpdateAllPositionsOnCanvas();
	void UpdateNodePositionOnCanvas(UHeartGraphCanvasNode* CanvasNode);

	void UpdateAllCanvasNodesZoom();

	void AddNodeToDisplay(UHeartGraphNode* Node);

	void SetViewOffset(const FVector2D& Value);
	void AddToViewOffset(const FVector2D& Value);

	void SetZoom(const double& Value);
	void AddToZoom(const double& Value);

	UFUNCTION()
	void OnNodeAddedToGraph(UHeartGraphNode* Node);

	UFUNCTION()
	void OnNodeRemovedFromGraph(UHeartGraphNode* Node);

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraph* GetGraph() const { return DisplayedGraph; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D GetViewOffset() const { return {View.X, View.Y}; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	double GetZoom() const { return View.Z; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D ScalePositionToCanvasZoom(const FVector2D& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D UnscalePositionToCanvasZoom(const FVector2D& Position) const;

	/** Set the displayed graph */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetGraph(UHeartGraph* Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetViewCorner(const FVector2D& NewViewCorner, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void AddToViewCorner(const FVector2D& NewViewCorner, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetZoom(double NewZoom, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void AddToZoom(double NewZoom, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraphCanvasPin* ResolvePinReference(const FHeartGraphPinReference& PinReference) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Heart|GraphCanvas|Events")
	FOnGraphViewChanged OnGraphViewChanged;

protected:
	/** The canvas to draw all nodes on. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, DisplayName = "CANVAS_Nodes"), Category = "Widgets")
	TObjectPtr<UCanvasPanel> NodeCanvas;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphCanvas")
	TObjectPtr<UHeartGraph> DisplayedGraph;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphCanvas")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphCanvasNode>> DisplayedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphCanvas")
	FHeartGraphPinReference PreviewConnectionPin;

	UPROPERTY(EditAnywhere, Category = "Heart|GraphCanvas")
	FHeartWidgetInputBindingContainer BindingContainer;

	// Multiplies movement to the view of the graph.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Heart|GraphCanvas|Config")
	FVector ViewMovementScalar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Heart|GraphCanvas|Config")
	FVectorBounds ViewBounds;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Heart|GraphCanvas|Config")
	float DraggingInterpSpeed = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Heart|GraphCanvas|Config")
	float ZoomInterpSpeed = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Heart|GraphCanvas|Config")
	EHeartGraphZoomAlgorithm ZoomBehavior;

private:
	FVector View;
	FVector TargetView;

	bool NeedsToUpdatePositions = false;
};
