// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartEvent.h"
#include "Model/HeartGraphInterface.h"
#include "View/HeartVisualizerInterfaces.h"
#include "SlatePointerWrappers.generated.h"

/**
 * This object is a wrapper around a SWidget shared pointer, to allow it to be passed through the heart linker
 */
UCLASS()
class UHeartSlatePtr : public UObject
{
	GENERATED_BODY()

public:
	static UHeartSlatePtr* Wrap(const TSharedRef<SWidget>& Widget);

protected:
	TSharedPtr<SWidget> SlatePointer;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IHeartGraphInterface
 */
UCLASS()
class UHeartSlateGraph : public UHeartSlatePtr, public IHeartGraphInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraph* GetHeartGraph() const override;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IGraphNodeVisualizerInterface
 */
UCLASS()
class UHeartSlateNode : public UHeartSlatePtr, public IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
};

/**
 * Child of HeartSlatePtr, whose sole purpose is to pass checks for implementing IGraphPinVisualizerInterface
 */
UCLASS()
class UHeartSlatePin : public UHeartSlatePtr, public IGraphPinVisualizerInterface
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
class UHeartSlateDragDropOperation : public UObject, public IHeartDeferredEventHandler
{
	GENERATED_BODY()

public:
	TSharedPtr<FDragDropOperation> SlatePointer;
};