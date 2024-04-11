// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "View/HeartVisualizerInterfaces.h"
#include "EdGraphPointerWrappers.generated.h"

/**
 * This object is a wrapper around an UEdGraphPin pointer, to allow it to be passed through heart linkers
 */
UCLASS()
class UHeartEdGraphPin : public UObject, public IGraphPinVisualizerInterface
{
	GENERATED_BODY()

public:
	static UHeartEdGraphPin* Wrap(const UEdGraphPin* Pin);

	virtual UHeartGraphNode* GetHeartGraphNode() const override;
	virtual FHeartPinGuid GetPinGuid() const override;

protected:
	const UEdGraphPin* EdGraphPin = nullptr;
	FHeartPinGuid PinGuid;
};