// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartLayoutHelper.generated.h"

class IHeartNodeLocationAccessor;
class UHeartGraphNode;

/**
 *
 */
UCLASS(Abstract, const, BlueprintType)
class HEART_API UHeartLayoutHelper : public UObject
{
	GENERATED_BODY()

public:
	// Required to override to implement logic. Returns false if no node locations were changed.
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<UHeartGraphNode*>& Nodes) const
		PURE_VIRTUAL(UHeartLayoutHelper::Layout, return false; )

	// Overload that calls Layout on all Nodes in the Graph.
	bool Layout(IHeartNodeLocationAccessor* Accessor) const;
};

UCLASS(Blueprintable)
class HEART_API UHeartLayoutHelper_BlueprintBase : public UHeartLayoutHelper
{
	GENERATED_BODY()

public:
	virtual bool Layout(IHeartNodeLocationAccessor* Accessor, const TArray<UHeartGraphNode*>& Nodes) const override final;

	UFUNCTION(BlueprintImplementableEvent, meta = (ScriptName = "Layout"))
	bool Layout_BP(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, const TArray<UHeartGraphNode*>& Nodes) const;
};
