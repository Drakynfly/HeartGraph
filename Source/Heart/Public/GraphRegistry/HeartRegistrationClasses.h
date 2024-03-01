// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartRegistrationClasses.generated.h"

class UHeartGraphNode;

USTRUCT(BlueprintType)
struct FHeartRegisteredClass
{
	GENERATED_BODY()

	// Register this class if it is not abstract.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = true))
	TObjectPtr<UClass> Class;

	// Register all non-abstract children of Class.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Recursive = false;
};

USTRUCT(BlueprintType)
struct FClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHeartRegisteredClass> Classes;
};

USTRUCT(BlueprintType)
struct FHeartObjectList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UObject>> Objects;
};

USTRUCT(BlueprintType)
struct FHeartRegistrationClasses
{
	GENERATED_BODY()

	// Maps graph node classes to object classes to register to be instanced as nodes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UHeartGraphNode>, FClassList> GraphNodeLists;

	// Maps graph node classes to individual objects to register to be placed as nodes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UHeartGraphNode>, FHeartObjectList> IndividualObjects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphConnectionVisualizerInterface"))
	TArray<TObjectPtr<UClass>> ConnectionVisualizerClasses;
};