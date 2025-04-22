// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphPinTag.h"

#include "HeartRegistryStructs.generated.h"

class UHeartGraphNode;

// DEPRECATED in favor of FHeartRegistryClass which uses Soft-referencing
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

// DEPRECATED in favor of FHeartRegistryNodeClasses which uses Soft-referencing
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

// DEPRECATED
USTRUCT()
struct FHeartObjectList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UObject>> Objects;
};

// DEPRECATED in favor of FHeartRegistryVisualizers which uses Soft-referencing
USTRUCT(BlueprintType)
struct FHeartPinClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphConnectionVisualizerInterface"))
	TArray<TObjectPtr<UClass>> ConnectionVisualizers;
};

// DEPRECATED in favor of FHeartRegistryClassLists which uses Soft-referencing
USTRUCT(BlueprintType)
struct FHeartRegistrationClasses
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses")
	TMap<TSubclassOf<UHeartGraphNode>, FHeartNodeClassList> GraphNodeLists;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses", meta = (ForceInlineRow))
	TMap<FHeartGraphPinTag, FHeartPinClassList> GraphPinLists;
};


USTRUCT(BlueprintType)
struct FHeartRegistryClass
{
	GENERATED_BODY()

	// Register this class if it is not abstract.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowAbstract = true))
	TSoftClassPtr<UObject> Class;

	// Register all non-abstract children of Class.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Recursive = false;

	// Register assets on disk that are instances of this class.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RegisterAssets = false;
};

USTRUCT(BlueprintType)
struct FHeartRegistryNodeClasses
{
	GENERATED_BODY()

	// Registered class node sources; used to create Instanced nodes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Class"))
	TArray<FHeartRegistryClass> Classes;

	// Registered object node sources; used to create Reference nodes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<UObject>> Objects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TSoftClassPtr<UObject>> Visualizers;
};

USTRUCT(BlueprintType)
struct FHeartRegistryVisualizers
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TSoftClassPtr<UObject>> PinVisualizers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "/Script/Heart.GraphConnectionVisualizerInterface"))
	TArray<TSoftClassPtr<UObject>> ConnectionVisualizers;
};

USTRUCT(BlueprintType)
struct FHeartRegistryClassLists
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses")
	TMap<TSoftClassPtr<UHeartGraphNode>, FHeartRegistryNodeClasses> NodeLists;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeartRegistrationClasses", meta = (ForceInlineRow))
	TMap<FHeartGraphPinTag, FHeartRegistryVisualizers> PinLists;
};