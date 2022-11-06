// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "HeartVisualizerInterfaces.h"
#include "ModelView/HeartGraphPin.h"
#include "UI/HeartWidgetInputLinkerRedirector.h"
#include "HeartGraphCanvasPin.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;

/**
 *
 */
UCLASS(Abstract)
class HEART_API UHeartGraphCanvasPin : public UHeartGraphWidgetBase,
	public IGraphPinVisualizerInterface, public IHeartWidgetInputLinkerRedirector
{
	GENERATED_BODY()

	friend UHeartGraphCanvasNode;

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker() const override;
	/** IHeartWidgetInputLinkerRedirector */

	// Is this pin the target of a connection, and may be connected if confirmed.
	void SetIsPreviewConnectionTarget(bool IsTarget, bool CanConnect);

	UHeartGraphPin* GetPin() const { return GraphPin.Get(); }
	UHeartGraphCanvasNode* GetCanvasNode() const { return GraphCanvasNode.Get(); }
	UHeartGraphCanvas* GetCanvas() const;

protected:
	// Called by DragDropOperation when connection pins. Useful to display a highlight on pins as if they are hovered.
	UFUNCTION(BlueprintImplementableEvent, Category = "HeartGraphCanvasPin")
	void DisplayPreviewConnectionTarget(bool IsTarget, bool CanConnect);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphCanvasPin")
	TWeakObjectPtr<UHeartGraphPin> GraphPin;

	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphCanvasPin")
	TWeakObjectPtr<UHeartGraphCanvasNode> GraphCanvasNode;
};
