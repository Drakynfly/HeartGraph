// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "Model/HeartGraphPinDesc.h"
#include "Model/HeartGraphPinReference.h"
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
	/** IHeartInputLinkerInterface */
	virtual UHeartInputLinkerBase* ResolveLinker_Implementation() const override final;
	/** IHeartInputLinkerInterface */

	/** IHeartGraphPinInterface */
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
	virtual FHeartPinGuid GetPinGuid() const override;
	/** IHeartGraphPinInterface */

	// Called by UHeartPinConnectionDragDropOperation when connecting pins.
	void SetIsPreviewConnectionTarget(bool IsTarget, bool CanConnect);

	FHeartGraphPinReference GetPinReference() const;

	const FHeartGraphPinDesc& GetPinDesc() const { return PinDescription; }

	UHeartGraphCanvasNode* GetCanvasNode() const { return GraphCanvasNode.Get(); }
	UHeartGraphCanvas* GetCanvas() const;

protected:
	// Is this pin the target of a connection, and may be connected if confirmed. Useful to display a highlight on pins as if they are hovered.
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphCanvasPin")
	void DisplayPreviewConnectionTarget(bool IsTarget, bool CanConnect);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GraphCanvasPin")
	FHeartPinGuid GraphPin;

	UPROPERTY(BlueprintReadOnly, Category = "GraphCanvasPin")
	FHeartGraphPinDesc PinDescription;

	UPROPERTY(BlueprintReadOnly, Category = "GraphCanvasPin")
	TWeakObjectPtr<UHeartGraphCanvasNode> GraphCanvasNode;
};