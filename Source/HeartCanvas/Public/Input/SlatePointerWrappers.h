// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartEvent.h"
#include "Model/HeartGraphInterface.h"
#include "View/HeartVisualizerInterfaces.h"
#include "SlatePointerWrappers.generated.h"

class FDragDropOperation;
class SWidget;

/**
 * This object is a wrapper around a SWidget shared pointer, to allow it to be passed through the heart linker
 */
UCLASS()
class HEARTCANVAS_API UHeartSlatePtr : public UObject
{
	GENERATED_BODY()

public:
	static UHeartSlatePtr* Wrap(const TSharedRef<SWidget>& Widget);

	TSharedPtr<SWidget> GetWidget() const { return SlatePointer; }

	template <class TWidgetType>
	TSharedPtr<TWidgetType> GetWidget() const { return StaticCastSharedPtr<TWidgetType>(SlatePointer); }

protected:
	TSharedPtr<SWidget> SlatePointer;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IHeartGraphInterface
 */
UCLASS()
class HEARTCANVAS_API UHeartSlateGraph : public UHeartSlatePtr, public IHeartGraphInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraph* GetHeartGraph() const override;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IGraphNodeVisualizerInterface
 */
UCLASS()
class HEARTCANVAS_API UHeartSlateNode : public UHeartSlatePtr, public IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IGraphPinVisualizerInterface
 */
UCLASS()
class HEARTCANVAS_API UHeartSlatePin : public UHeartSlatePtr, public IGraphPinVisualizerInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
	virtual FHeartPinGuid GetPinGuid() const override;
};

/**
 * This object is a wrapper around a FDragDropOperation shared pointer, to allow it to be passed through the heart linker
 */
UCLASS()
class HEARTCANVAS_API UHeartSlateDragDropOperation : public UObject, public IHeartDeferredEventHandler
{
	GENERATED_BODY()

public:
	TSharedPtr<FDragDropOperation> SlatePointer;
};