// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartLayoutHelper.generated.h"

struct FHeartNodeGuid;
class IHeartNodeLocationInterface;
class UHeartGraph;

USTRUCT()
struct FHeartGraphAdjacencyList
{
	GENERATED_BODY()

	TArray<TArray<int32>> AdjacencyList;
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, EditInlineNew)
class HEART_API UHeartLayoutHelper : public UObject
{
	GENERATED_BODY()

public:
	// Required to override to implement logic. Returns false if no node locations were changed.
	virtual bool Layout(TNotNull<UHeartGraph*> Graph, IHeartNodeLocationInterface& Interface, const TArray<FHeartNodeGuid>& Nodes)
		PURE_VIRTUAL(UHeartLayoutHelper::Layout, return false; )

	// Overload that consumes a DeltaTime, for layouts that handle being run on tick
	virtual bool Layout(TNotNull<UHeartGraph*> Graph, IHeartNodeLocationInterface& Interface, const TArray<FHeartNodeGuid>& Nodes, float DeltaTime)
	{
		return Layout(Graph, Interface, Nodes);
	}

	// Overload that calls Layout on all Nodes in the Graph.
	bool Layout(TNotNull<UHeartGraph*> Graph, IHeartNodeLocationInterface& Interface);
	bool Layout(TNotNull<UHeartGraph*> Graph, IHeartNodeLocationInterface& Interface, float DeltaTime);

protected:
	static FHeartGraphAdjacencyList GetGraphAdjacencyList(const TNotNull<UHeartGraph*> Graph, const TArray<FHeartNodeGuid>& Nodes);

	UFUNCTION(BlueprintCallable, Category = "Heart|LayoutHelper")
	void ApplyNewPositions(const TScriptInterface<IHeartNodeLocationInterface>& Interface, const TArray<FHeartNodeGuid>& Nodes, const TArray<FVector2D>& NewPositions) const;
};

UCLASS(Abstract, Blueprintable, MinimalAPI)
class UHeartLayoutHelper_BlueprintBase final : public UHeartLayoutHelper
{
	GENERATED_BODY()

protected:
	virtual bool Layout(TNotNull<UHeartGraph*> Graph, IHeartNodeLocationInterface& Interface, const TArray<FHeartNodeGuid>& Nodes) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (ScriptName = "Layout"))
	bool Layout_BP(UHeartGraph* Graph, const TScriptInterface<IHeartNodeLocationInterface>& Interface, const TArray<FHeartNodeGuid>& Nodes);
};