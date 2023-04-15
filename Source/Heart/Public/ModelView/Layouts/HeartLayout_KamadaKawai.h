// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartLayoutHelper.h"
#include "HeartLayout_KamadaKawai.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartLayout_KamadaKawai : public UHeartLayoutHelper
{
	GENERATED_BODY()

public:
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<UHeartGraphNode*>& Nodes) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart|KamadaKawai")
	int32 Width = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart|KamadaKawai")
	int32 Height = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart|KamadaKawai")
	double Strength = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart|KamadaKawai")
	double EnergyThreshold = 0.01;
};
