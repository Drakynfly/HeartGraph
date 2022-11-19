// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "General/VectorBounds.h"
#include "Model/HeartGraphPinReference.h"
#include "ModelView/HeartGraphNode.h"
#include "HeartGraphCanvas.generated.h"

class UHeartGraphCanvasPanel;
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

USTRUCT(BlueprintType)
struct FHeartDragIntoViewSettings
{
	GENERATED_BODY()

	// Allow dragging a node into the corner of the canvas to pan the canvas into view.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings")
	bool EnableDragIntoView = false;

	// Animate drag into view
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings")
	bool InterpDragIntoView = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings")
	float DragMultiplier = 0.1;

	// Drag-into-view input is clamped to this magnitude.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings")
	float DragIntoViewClamp = 10;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGraphViewChanged);

/**
 * Base class for displaying a graph using UMG widgets on a canvas panel.
 */
UCLASS()
class HEARTCANVAS_API UHeartGraphCanvas : public UHeartGraphWidgetBase
{
	GENERATED_BODY()

public:
	UHeartGraphCanvas(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** UUserWidget */
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	/** UUserWidget */

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override;
	/** IHeartWidgetInputLinkerRedirector */

	// Used by UHeartPinConnectionDragDropOperation to notify us about what its doing so we can draw the preview link
	void SetPreviewConnection(const FHeartGraphPinReference& Reference);

protected:
	bool IsNodeCulled(UHeartGraphCanvasNode* GraphNode, const FGeometry& Geometry) const;

	void Reset();

	void Refresh();

	/** Update the position of every node on the canvas. */
	void UpdateAllPositionsOnCanvas();
	void UpdateNodePositionOnCanvas(const UHeartGraphCanvasNode* CanvasNode);

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

	UFUNCTION()
	void OnNodeLocationChanged(UHeartGraphNode* Node, const FVector2D& Location);


	/****************************/
	/**		GETTERS				*/
	/****************************/
public:
	template <typename THeartGraph>
	THeartGraph* GetGraph() const
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "The graph class must derive from UHeartGraph");
		return Cast<THeartGraph>(DisplayedGraph);
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraph* GetGraph() const { return DisplayedGraph.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas", meta = (DeterminesOutputType = Class))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return DisplayedGraph.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D GetViewOffset() const { return {View.X, View.Y}; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	double GetZoom() const { return View.Z; }

	const FHeartDragIntoViewSettings& GetDragIntoViewSettings() const { return DragIntoViewSettings; }


	/****************************/
	/**		UTILITIES			*/
	/****************************/

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D ScalePositionToCanvasZoom(const FVector2D& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D UnscalePositionToCanvasZoom(const FVector2D& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraphCanvasPin* ResolvePinReference(const FHeartGraphPinReference& PinReference) const;


	/****************************/
	/**		GRAPH VIEWING		*/
	/****************************/

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


	/****************************/
	/**		NODE SELECTION		*/
	/****************************/

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SelectNode(FHeartNodeGuid Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SelectNodes(const TArray<FHeartNodeGuid>& Nodes);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void UnselectNode(FHeartNodeGuid Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void ClearNodeSelection();


public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphViewChanged OnGraphViewChanged;

protected:
	/** The canvas to draw all nodes on. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, DisplayName = "CANVAS_Nodes"), Category = "Widgets")
	TObjectPtr<UHeartGraphCanvasPanel> NodeCanvas;

	UPROPERTY()
	TWeakObjectPtr<UHeartGraph> DisplayedGraph;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphCanvasNode>> DisplayedNodes;

	UPROPERTY(EditAnywhere, Category = "Input")
	FHeartWidgetInputBindingContainer BindingContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TSet<FHeartNodeGuid> SelectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FHeartGraphPinReference PreviewConnectionPin;

	// Multiplies movement to the view of the graph.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FVector ViewMovementScalar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FVectorBounds ViewBounds;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	float DraggingInterpSpeed = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	float ZoomInterpSpeed = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	EHeartGraphZoomAlgorithm ZoomBehavior;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FHeartDragIntoViewSettings DragIntoViewSettings;

private:
	FVector View;
	FVector TargetView;

	bool NeedsToUpdatePositions = false;
};
