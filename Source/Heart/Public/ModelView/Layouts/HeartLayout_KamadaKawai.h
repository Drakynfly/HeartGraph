// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartLayoutHelper.h"
#include "HeartLayout_KamadaKawai.generated.h"

/**
 * A simple layout implementation for a one-shot Kamada-Kawai run.
 */
UCLASS(DisplayName = "Heart Layout Kamada-Kawai")
class HEART_API UHeartLayout_KamadaKawai : public UHeartLayoutHelper
{
	GENERATED_BODY()

public:
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 Width = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 Height = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Strength = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double EnergyThreshold = 0.01;
};