// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartLayoutHelper.generated.h"

struct FHeartNodeGuid;
class IHeartGraphInterface;
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
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes)
		PURE_VIRTUAL(UHeartLayoutHelper::Layout, return false; )

	// Overload that consumes a DeltaTime, for layouts that handle being run on tick
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes, float DeltaTime)
	{
		return Layout(Interface, Nodes);
	}

	// Overload that calls Layout on all Nodes in the Graph.
	bool Layout(IHeartGraphInterface* Interface);
	bool Layout(IHeartGraphInterface* Interface, float DeltaTime);

protected:
	static FHeartGraphAdjacencyList GetGraphAdjacencyList(const IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes);

	UFUNCTION(BlueprintCallable, Category = "Heart|LayoutHelper")
	void ApplyNewPositions(const TScriptInterface<IHeartGraphInterface>& Interface, const TArray<FHeartNodeGuid>& Nodes, const TArray<FVector2D>& NewPositions) const;
};

UCLASS(Abstract, Blueprintable, MinimalAPI)
class UHeartLayoutHelper_BlueprintBase final : public UHeartLayoutHelper
{
	GENERATED_BODY()

protected:
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (ScriptName = "Layout"))
	bool Layout_BP(const TScriptInterface<IHeartGraphInterface>& Interface, const TArray<FHeartNodeGuid>& Nodes);
};