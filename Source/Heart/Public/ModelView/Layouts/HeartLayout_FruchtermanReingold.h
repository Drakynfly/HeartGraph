// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Algorithms/FruchtermanReingold.h"
#include "ModelView/HeartLayoutHelper.h"
#include "HeartLayout_FruchtermanReingold.generated.h"

/**
 * An iterative layout implementation of Fruchterman-Reingold that runs on tick.
 */
UCLASS(DisplayName = "Heart Layout Fruchterman-Reingold")
class HEART_API UHeartLayout_FruchtermanReingold : public UHeartLayoutHelper
{
	GENERATED_BODY()

public:
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes, float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Strength = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (UIMin = 1, UIMax = 300))
	int32 IterationsPerSecond = 60;

	FHeartGraphAdjacencyList AdjacencyList;

	TOptional<Nodesoup::FruchtermanReingold> Algorithm;

	float Accum = 0;
};