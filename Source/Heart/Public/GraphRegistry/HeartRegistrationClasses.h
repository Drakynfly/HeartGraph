// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphPinTag.h"

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
struct FHeartNodeClassList
{
	GENERATED_BODY()

	// Registered class node sources; used to create Instanced nodes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Class"))
	TArray<FHeartRegisteredClass> Classes;

	// Registered object node sources; used to create Reference nodes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UObject>> Objects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> Visualizers;
};

USTRUCT()
struct FHeartObjectList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UObject>> Objects;
};

USTRUCT(BlueprintType)
struct FHeartPinClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphConnectionVisualizerInterface"))
	TArray<TObjectPtr<UClass>> ConnectionVisualizers;
};

USTRUCT(BlueprintType)
struct FHeartRegistrationClasses
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses")
	TMap<TSubclassOf<UHeartGraphNode>, FHeartNodeClassList> GraphNodeLists;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses", meta = (ForceInlineRow))
	TMap<FHeartGraphPinTag, FHeartPinClassList> GraphPinLists;

	UE_DEPRECATED(5.3, "Use GraphNodeLists.Objects instead")
	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<UHeartGraphNode>, FHeartObjectList> IndividualObjects;

	UE_DEPRECATED(5.3, "Use GraphNodeLists.Visualizers instead")
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UE_DEPRECATED(5.3, "Use GraphPinLists.PinVisualizers instead")
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;
};