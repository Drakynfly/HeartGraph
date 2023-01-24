// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Model/HeartGuids.h"
#include "InstancedStruct.h"
#include "Model/HeartPinDirection.h"
#include "HeartNodeSortingLibrary.generated.h"

USTRUCT(BlueprintType)
struct HEART_API FHeartNodeLayer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSet<FHeartNodeGuid> Nodes;
};

USTRUCT(BlueprintType)
struct HEART_API FHeartTreeNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FHeartNodeGuid Node;

	UPROPERTY(BlueprintReadOnly, meta = (BaseStruct = "/Script/Heart.HeartTreeNode"))
	TArray<FInstancedStruct> Children;
};

USTRUCT(BlueprintType)
struct HEART_API FHeartTree
{
	GENERATED_BODY()

	UPROPERTY()
	FHeartTreeNode RootNode;
};

USTRUCT(BlueprintType)
struct HEART_API FNodeLooseToTreeArgs
{
	GENERATED_BODY()

	// Allow the same node to appear in multiple places in the tree.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AllowDuplicates = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHeartPinDirection Direction;
};

/**
 *
 */
UCLASS()
class HEART_API UHeartNodeSortingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SortLooseNodesIntoTrees(const TArray<UHeartGraphNode*>& Nodes, const FNodeLooseToTreeArgs& Args, TArray<FHeartTree>& Trees);

	UFUNCTION(BlueprintCallable)
	static void ConvertNodeTreeToLayers(const FHeartTree& Tree, TArray<FHeartNodeLayer>& Layers);
};
