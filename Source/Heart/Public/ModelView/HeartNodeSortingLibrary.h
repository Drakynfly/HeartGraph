// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Model/HeartGuids.h"
#include "Model/HeartPinDirection.h"
#include "StructUtils/InstancedStruct.h"
#include "HeartNodeSortingLibrary.generated.h"

class IHeartGraphInterface;
class UHeartGraph;
class UHeartGraphNode;

USTRUCT(BlueprintType)
struct HEART_API FHeartNodeSet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSet<FHeartNodeGuid> Nodes;
};

/*
 * A node layers is a group of nodes all coming from a certain "depth" in a tree.
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartNodeLayer : public FHeartNodeSet
{
	GENERATED_BODY()
};

/*
 * A node path is a series of nodes known to be connection input to output.
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartNodePath : public FHeartNodeSet
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct HEART_API FHeartTreeNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FHeartNodeGuid Node;

	UPROPERTY(BlueprintReadOnly)
	TArray<TInstancedStruct<FHeartTreeNode>> Children;
};

USTRUCT(BlueprintType)
struct HEART_API FHeartTree
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<const UHeartGraph> Graph;

	UPROPERTY()
	FHeartTreeNode RootNode;
};

USTRUCT(BlueprintType)
struct HEART_API FNodeLooseToTreeArgs
{
	GENERATED_BODY()

	// Allow the same node to appear in multiple places in the tree.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AllowDuplicates = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHeartPinDirection Direction = EHeartPinDirection::None;
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FHeartNodeFilterPredicate, const FHeartNodeGuid&, Node);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FHeartNodeComparePredicate, const FHeartNodeGuid&, A, const FHeartNodeGuid&, B);

/**
 * @todo make namespace version and move this to private
 */
UCLASS()
class HEART_API UHeartNodeSortingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary")
	static TArray<UHeartGraphNode*> ResolveNodes(const UHeartGraph* Graph, const TArray<FHeartNodeGuid>& Nodes);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary")
	static TArray<FHeartNodeGuid> SortNodes(const TArray<FHeartNodeGuid>& Nodes, const FHeartNodeComparePredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary")
	static void SortNodesInPlace(UPARAM(ref) TArray<FHeartNodeGuid>& Nodes, const FHeartNodeComparePredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary", meta = (DisplayName = "Filter Nodes (Predicate)"))
	static TArray<FHeartNodeGuid> FilterNodesByPredicate(const TArray<FHeartNodeGuid>& Nodes, const FHeartNodeFilterPredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary", meta = (DisplayName = "Filter Nodes Exclusive (Predicate)"))
	static TArray<FHeartNodeGuid> FilterNodesByPredicate_Exclusive(const TArray<FHeartNodeGuid>& Nodes, const FHeartNodeFilterPredicate& Predicate);

	// Filters nodes to only return those of the given class set.
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary", meta = (DisplayName = "Filter Nodes (Class)", DeterminesOutputType = "Classes"))
	static TArray<FHeartNodeGuid> FilterNodesByClass(const TScriptInterface<IHeartGraphInterface> Graph,
		const TArray<FHeartNodeGuid>& Nodes, const TSet<TSubclassOf<UHeartGraphNode>>& Classes);

	// Filters nodes to only return those *not* of the given class set.
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary", meta = (DisplayName = "Filter Nodes Exclusive (Class)"))
	static TArray<FHeartNodeGuid> FilterNodesByClass_Exclusive(const TScriptInterface<IHeartGraphInterface> Graph,
		const TArray<FHeartNodeGuid>& Nodes, const TSet<TSubclassOf<UHeartGraphNode>>& Classes);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary")
	static void SortLooseNodesIntoTrees(TScriptInterface<IHeartGraphInterface> Graph, const TArray<FHeartNodeGuid>& Nodes,
		const FNodeLooseToTreeArgs& Args, TArray<FHeartTree>& Trees);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeSortingLibrary")
	static void ConvertNodeTreeToLayers(const FHeartTree& Tree, TArray<FHeartNodeLayer>& Layers);
};