// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"

#include "Input/HeartWidgetInputBindingContainer.h"
#include "Model/HeartGraphPinReference.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartNodeLocationAccessor.h"

#include "General/VectorBounds.h"

#include "HeartGraphCanvas.generated.h"

class UCanvasPanelSlot;

class UHeartGraph;
class UHeartGraphCanvasPanel;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;
class UHeartGraphCanvasConnection;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraphCanvas, Log, All)

UENUM()
enum class EHeartGraphZoomAlgorithm : uint8
{
	// No compensation to view will be applied when zooming
	None,

	// Adjust the view to maintain mouse position
	MouseRelative,

	// Adjust the view to maintain graph position
	GraphRelative,
};

UENUM()
enum class EHeartGraphCanvasInvalidateType : uint8
{
	// Completely rebuild of the canvas widget
	Full,

	// Update canvas node with underlying node's location
	NodeLocation,

	// Rebuild node connections
	Connections,
};

USTRUCT(BlueprintType)
struct FHeartDragIntoViewSettings
{
	GENERATED_BODY()

	// Allow dragging a node into the corner of the canvas to pan the canvas into view.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings")
	bool EnableDragIntoView = false;

	// Animate drag into view
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings", meta = (EditCondition = "EnableDragIntoView"))
	bool InterpDragIntoView = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings", meta = (EditCondition = "EnableDragIntoView"))
	float DragMultiplier = 0.1;

	// Drag-into-view input is clamped to this magnitude.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartDragIntoViewSettings", meta = (EditCondition = "EnableDragIntoView"))
	float DragIntoViewClamp = 10;
};

USTRUCT(BlueprintType)
struct FHeartPanToSelectionSettings
{
	GENERATED_BODY()

	// Automatically pan the graph view to center the selected nodes.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartPanToSelectionSettings")
	bool EnablePanToSelection = false;

	// Automatically adjust zoom to focus on the selected nodes.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartPanToSelectionSettings")
	bool EnableZoomToSelection = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartPanToSelectionSettings", meta = (EditCondition = "EnableZoomToSelection"))
	float ZoomDistance = 1.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGraphViewChanged);

/**
 * Base class for displaying a graph using UMG widgets on a canvas panel.
 */
UCLASS()
class HEARTCANVAS_API UHeartGraphCanvas : public UHeartGraphWidgetBase, public IHeartNodeLocationAccessor
{
	GENERATED_BODY()

public:
	UHeartGraphCanvas();

protected:
	/** UUserWidget */
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	/** UUserWidget */

public:
	/** IHeartInputLinkerInterface */
	virtual UHeartInputLinkerBase* ResolveLinker_Implementation() const override;
	/** IHeartInputLinkerInterface */

	/** IHeartGraphInterface */
	virtual UHeartGraph* GetHeartGraph() const override;
	/** IHeartGraphInterface */

	/** IHeartNodeLocationAccessor */
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const override;
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove) override;
	/** IHeartNodeLocationAccessor */


	// Used by UHeartPinConnectionDragDropOperation to notify us about what its doing, so we can draw the preview link
	void SetPreviewConnection(const FHeartGraphPinReference& Reference);

	UCanvasPanelSlot* AddConnectionWidget(UHeartGraphCanvasConnection* ConnectionWidget);

	/**
	 * Get the class used to display a node on the Canvas Graph. This has a default implementation that fetches a
	 * visualizer from the Runtime Subsystem Registry for the graph. Override to provide alternate/custom behavior.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphCanvas")
	TSubclassOf<UHeartGraphCanvasNode> GetVisualClassForNode(const UHeartGraphNode* Node) const;

	/**
	 * Get the class used to display the node connections on the Canvas Graph. This has a default implementation that fetches a
	 * visualizer from the Runtime Subsystem Registry for the graph. Override to provide alternate/custom behavior.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphCanvas")
	TSubclassOf<UHeartGraphCanvasConnection> GetVisualClassForConnection(const FHeartGraphPinDesc& FromDesc, const FHeartGraphPinDesc& ToDesc) const;

	/**
	 * Get the class used to display the preview connection on the Canvas Graph. This has a default implementation that fetches a
	 * visualizer from the Runtime Subsystem Registry for the graph. Override to provide alternate/custom behavior.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphCanvas")
	TSubclassOf<UHeartGraphCanvasConnection> GetVisualClassForPreviewConnection() const;

protected:
	bool IsNodeCulled(const UHeartGraphCanvasNode* GraphNode, const FGeometry& Geometry) const;

	void Reset();

	void Refresh();

	/** Update the position of every node on the canvas. */
	void UpdateAllPositionsOnCanvas();
	void UpdateNodePositionOnCanvas(const UHeartGraphCanvasNode* CanvasNode);

	void UpdateAllCanvasNodesZoom();

	void UpdateAfterSelectionChanged();

	void CreatePreviewConnection();

	void AddNodeToDisplay(UHeartGraphNode* Node, bool InitNodeWidget);

	void SetViewOffset(const FVector2f& Value);
	void AddToViewOffset(const FVector2f& Value);

	void SetZoom(float Value);
	void AddToZoom(float Value);

	UFUNCTION()
	void OnNodeAddedToGraph(UHeartGraphNode* Node);

	UFUNCTION()
	void OnNodeRemovedFromGraph(UHeartGraphNode* Node);

	UFUNCTION()
	void OnNodeLocationChanged(UHeartGraphNode* Node, const FVector2D& Location);


	/*----------------------
			GETTERS
	----------------------*/
public:
	template <
		typename THeartGraph
		UE_REQUIRES(TIsDerivedFrom<THeartGraph, UHeartGraph>::Value)
	>
	THeartGraph* GetGraph() const
	{
		return Cast<THeartGraph>(DisplayedGraph);
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraph* GetGraph() const { return DisplayedGraph.Get(); }

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = Class, DeprecatedFunction, DeprecatedMessage = "Please use version in utils library instead"))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return DisplayedGraph.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D GetViewOffset() const { return {View.X, View.Y}; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	float GetZoom() const { return View.Z; }

	const FHeartDragIntoViewSettings& GetDragIntoViewSettings() const { return DragIntoViewSettings; }


	/*----------------------
			UTILITIES
	----------------------*/

	FVector2f ScalePositionToCanvasZoom_2f(const FVector2f& Position) const;
	FVector2f UnscalePositionToCanvasZoom_2f(const FVector2f& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D ScalePositionToCanvasZoom(const FVector2D& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	FVector2D UnscalePositionToCanvasZoom(const FVector2D& Position) const;

	// Reconstruct the display for this node. Useful for forcing updates on nodes that are known to have changed, but
	// don't have specific bindings available. Try not to use Type = Full too much, however, as this isn't the cheapest operation.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void InvalidateNodeDisplay(const FHeartNodeGuid& NodeGuid, EHeartGraphCanvasInvalidateType Type = EHeartGraphCanvasInvalidateType::Full);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraphCanvasPin* ResolvePinReference(const FHeartGraphPinReference& PinReference) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	UHeartGraphCanvasNode* GetCanvasNode(const FHeartNodeGuid& NodeGuid);


	/*---------------------------
			GRAPH VIEWING
	---------------------------*/

	void AddToViewCorner(const FVector2f& NewViewCorner, bool Interp);

	/** Set the displayed graph */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetGraph(UHeartGraph* Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetViewCorner(const FVector2D& NewViewCorner, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void AddToViewCorner(const FVector2D& NewViewCorner, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SetZoom(float NewZoom, bool Interp);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void AddToZoom(float NewZoom, bool Interp);


	/*---------------------------
			NODE SELECTION
	---------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SelectNode(const FHeartNodeGuid& Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void SelectNodes(const TArray<FHeartNodeGuid>& Nodes);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void UnselectNode(const FHeartNodeGuid& Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	bool IsNodeSelected(const FHeartNodeGuid& Node) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas")
	void ClearNodeSelection();


	/*---------------------------
			POP UP UTILS
	---------------------------*/

	UFUNCTION(BlueprintCallable)
	UCanvasPanelSlot* AddWidgetToPopups(UWidget* Widget, FVector2D Location);

	UFUNCTION(BlueprintCallable)
	bool RemoveWidgetFromPopups(UWidget* Widget);

	UFUNCTION(BlueprintCallable)
	void ClearPopups();


	/*---------------------------
			CANVAS STATE
	---------------------------*/
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphViewChanged OnGraphViewChanged;

protected:
	/** The canvas to draw widgets on. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, DisplayName = "CANVAS_Nodes"), Category = "Widgets")
	TObjectPtr<UHeartGraphCanvasPanel> NodeCanvas;

	UPROPERTY()
	TWeakObjectPtr<UHeartGraph> DisplayedGraph;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphCanvasNode>> DisplayedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<TObjectPtr<UWidget>> Popups;

	// Widget used to draw preview connections. Only valid when PreviewConnectionPin is set.
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UHeartGraphCanvasConnection> PreviewConnection;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "Widgets", NoClear, meta = (ShowInnerProperties))
	TObjectPtr<UHeartNodeLocationModifierStack> LocationModifiers;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (ShowOnlyInnerProperties))
	FHeartWidgetInputBindingContainer BindingContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TSet<FHeartNodeGuid> SelectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FHeartGraphPinReference PreviewConnectionPin;


	/*----------------------------
			 VISUAL CONFIG
	----------------------------*/

	// Multiplies movement to the view of the graph.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FVector3f ViewMovementScalar;

	// X and Y bounds limit panning. Z bound limits zoom.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FVector3fBounds ViewBounds;

	// Interpolation applied while panning.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	float DraggingInterpSpeed = 0;

	// Interpolation applied while changing zoom.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	float ZoomInterpSpeed = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	EHeartGraphZoomAlgorithm ZoomBehavior;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FHeartDragIntoViewSettings DragIntoViewSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FHeartPanToSelectionSettings PanToSelectionSettings;

	// @todo temp until everything is moved over to use connection widgets
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	bool UseDeprecatedPaintMethodToDrawConnections = false;

private:
	FVector3f View;
	FVector3f TargetView;

	bool NeedsToUpdatePositions = false;
};