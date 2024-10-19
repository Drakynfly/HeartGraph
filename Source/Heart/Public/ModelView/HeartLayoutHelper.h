// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartLayoutHelper.generated.h"

struct FHeartNodeGuid;
class IHeartNodeLocationAccessor;
class UHeartGraphNode;

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
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes)
		PURE_VIRTUAL(UHeartLayoutHelper::Layout, return false; )

	// Overload that consumes a DeltaTime, for layouts that handle being run on tick
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes, float DeltaTime)
	{
		return Layout(Accessor, Nodes);
	}

	// Overload that calls Layout on all Nodes in the Graph.
	bool Layout(IHeartNodeLocationAccessor* Accessor);
	bool Layout(IHeartNodeLocationAccessor* Accessor, float DeltaTime);

protected:
	FHeartGraphAdjacencyList GetGraphAdjacencyList(const IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|LayoutHelper")
	void ApplyNewPositions(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, const TArray<FHeartNodeGuid>& Nodes, const TArray<FVector2D>& NewPositions) const;
};

UCLASS(Abstract, Blueprintable, MinimalAPI)
class UHeartLayoutHelper_BlueprintBase final : public UHeartLayoutHelper
{
	GENERATED_BODY()

protected:
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<FHeartNodeGuid>& Nodes) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (ScriptName = "Layout"))
	bool Layout_BP(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, const TArray<FHeartNodeGuid>& Nodes);
};