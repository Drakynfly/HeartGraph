// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphNodeComponent.h"
#include "Model/HeartNodeComponentPinProvider.h"
#include "HeartDynamicPinsComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEART_API UHeartDynamicPinsComponent : public UHeartGraphNodeComponent, public IHeartNodeComponentPinProvider
{
	GENERATED_BODY()

public:
	//~ IHeartNodeComponentPinProvider
	virtual void GatherPins(TArray<FHeartGraphPinDesc>& Pins) const override;
	//~ IHeartNodeComponentPinProvider

	TConstArrayView<FHeartGraphPinDesc> GetPins() const { return DynamicPins; }

protected:
	UPROPERTY(EditAnywhere, Category = "Pins")
	TArray<FHeartGraphPinDesc> DynamicPins;
};
