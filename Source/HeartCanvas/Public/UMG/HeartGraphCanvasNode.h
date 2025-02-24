﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "Model/HeartGuids.h"
#include "View/HeartVisualizerInterfaces.h"
#include "HeartGraphCanvasNode.generated.h"

class UHeartGraphNode;
class UHeartGraphCanvas;
class UHeartGraphCanvasPin;
class UHeartGraphCanvasConnection;

/**
 *
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartGraphCanvasNode : public UHeartGraphWidgetBase, public IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

	friend UHeartGraphCanvas;

public:
	virtual void NativeDestruct() override;

	/** IHeartInputLinkerInterface */
	virtual UHeartInputLinkerBase* ResolveLinker_Implementation() const override;
	/** IHeartInputLinkerInterface */

	/** IHeartGraphNodeInterface */
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
	/** IHeartGraphNodeInterface */

protected:
	virtual void PostInitNode();

	virtual void SetNodeSelectedFromGraph(bool Selected);

public:
	UHeartGraphNode* GetGraphNode() const { return GraphNode.Get(); }
	UHeartGraphCanvas* GetCanvas() const { return GraphCanvas.Get(); }
	bool IsNodeSelected() const { return NodeSelected; }

	void SetNodeSelected(bool Selected);

	void RebuildAllPinConnections();

	void RebuildPinConnections(const FHeartPinGuid& Pin);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = Class, DeprecatedFunction))
	UHeartGraphNode* GetNodeTyped(TSubclassOf<UHeartGraphNode> Class) const { return GraphNode.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	UHeartGraphCanvasPin* GetPinWidget(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphCanvasNode")
	TArray<UHeartGraphCanvasPin*> GetPinWidgets() const { return PinWidgets; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	UHeartGraphCanvasPin* CreatePinWidget(const FHeartPinGuid& Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	void DestroyPinWidget(UHeartGraphCanvasPin* PinWidget);

	UFUNCTION(BlueprintImplementableEvent)
	void OnZoomSet(double Zoom);

protected:
	// UMG hook to display selection status
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnNodeSelectionChanged();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Node")
	TWeakObjectPtr<UHeartGraphNode> GraphNode;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TWeakObjectPtr<UHeartGraphCanvas> GraphCanvas;

	// All pins widgets contained by this node
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<TObjectPtr<UHeartGraphCanvasPin>> PinWidgets;

	// All connection widgets leading *from* this node to others.
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<TObjectPtr<UHeartGraphCanvasConnection>> ConnectionWidgets;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool NodeSelected = false;
};