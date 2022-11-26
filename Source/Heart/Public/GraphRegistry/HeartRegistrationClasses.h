// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartRegistrationClasses.generated.h"

class UHeartGraphNode;

USTRUCT(BlueprintType)
struct FHeartRegistrationClasses
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false))
	TArray<TObjectPtr<UClass>> NodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false))
	TArray<TSubclassOf<UHeartGraphNode>> GraphNodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false, MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false, MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;
};