// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "HeartVisualizerInterfaces.h"
#include "UI/HeartWidgetInputLinkerRedirector.h"
#include "HeartGraphCanvasNode.generated.h"

class UHeartGraphNode;
class UHeartGraphCanvas;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract)
class HEART_API UHeartGraphCanvasNode : public UHeartGraphWidgetBase,
	public IGraphNodeVisualizerInterface, public IHeartWidgetInputLinkerRedirector
{
	GENERATED_BODY()

	friend UHeartGraphCanvas;

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker() const override;
	/** IHeartWidgetInputLinkerRedirector */

	UHeartGraphCanvas* GetCanvas() const { return GraphCanvas.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	UHeartGraphNode* GetNode() const { return GraphNode.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	UHeartGraphCanvasPin* GetPinWidget(const FHeartPinGuid& Guid) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphCanvasNode")
	TArray<UHeartGraphCanvasPin*> GetPinWidgets() const { return PinWidgets; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode")
	UHeartGraphCanvasPin* CreatePinWidget(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintImplementableEvent)
	void OnZoomSet(double Zoom);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphCanvasNode")
	TWeakObjectPtr<UHeartGraphNode> GraphNode;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphCanvasNode")
	TWeakObjectPtr<UHeartGraphCanvas> GraphCanvas;

	UPROPERTY(BlueprintReadWrite, Category = "Heart|GraphCanvasNode")
	TArray<TObjectPtr<UHeartGraphCanvasPin>> PinWidgets;
};
