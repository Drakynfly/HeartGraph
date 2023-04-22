// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "Model/HeartGraphPin.h"
#include "View/HeartVisualizerInterfaces.h"
#include "HeartGraphCanvasPin.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;

/**
 *
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartGraphCanvasPin : public UHeartGraphWidgetBase, public IGraphPinVisualizerInterface
{
	GENERATED_BODY()

	friend UHeartGraphCanvasNode;

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override final;
	/** IHeartWidgetInputLinkerRedirector */

	/** IHeartGraphPinInterface */
	virtual UHeartGraphNode* GetNode_Implementation() override;
	virtual FHeartPinGuid GetPinGuid_Implementation() const override;
	/** IHeartGraphPinInterface */

	// Called by UHeartPinConnectionDragDropOperation when connecting pins.
	void SetIsPreviewConnectionTarget(bool IsTarget, bool CanConnect);

	UHeartGraphPin* GetPin() const { return GraphPin.Get(); }
	UHeartGraphCanvasNode* GetCanvasNode() const { return GraphCanvasNode.Get(); }
	UHeartGraphCanvas* GetCanvas() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasPin", meta = (DeterminesOutputType = Class))
	UHeartGraphPin* GetPinTyped(TSubclassOf<UHeartGraphPin> Class) const { return GraphPin.Get(); }

protected:
	// Is this pin the target of a connection, and may be connected if confirmed. Useful to display a highlight on pins as if they are hovered.
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphCanvasPin")
	void DisplayPreviewConnectionTarget(bool IsTarget, bool CanConnect);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GraphCanvasPin")
	TWeakObjectPtr<UHeartGraphPin> GraphPin;

	UPROPERTY(BlueprintReadOnly, Category = "GraphCanvasPin")
	TWeakObjectPtr<UHeartGraphCanvasNode> GraphCanvasNode;
};
