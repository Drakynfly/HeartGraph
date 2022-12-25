﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartRegistrationClasses.generated.h"

class UHeartGraphNode;

USTRUCT(BlueprintType)
struct FClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false, BlueprintBaseOnly))
	TArray<TObjectPtr<UClass>> NodeClasses;
};

USTRUCT(BlueprintType)
struct FHeartRegistrationClasses
{
	GENERATED_BODY()

	// Known node classes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = false, BlueprintBaseOnly))
	TArray<TObjectPtr<UClass>> NodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UHeartGraphNode>> GraphNodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UHeartGraphNode>, FClassList> GraphNodeLists;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;
};