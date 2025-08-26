// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphComponentBase.h"
#include "HeartGraphNodeComponent.generated.h"

class UHeartGraph;

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class HEART_API UHeartGraphNodeComponent : public UHeartGraphComponentBase
{
	GENERATED_BODY()

	friend class UHeartGraph;

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeComponent")
	UHeartGraph* GetGraph() const;
};


USTRUCT()
struct FHeartGraphNodeComponentMap
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "NodeComponents")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNodeComponent>> Components;

	TObjectPtr<UHeartGraphNodeComponent> Find(const FHeartNodeGuid& Node) const
	{
		if (auto&& NodePtr = Components.Find(Node))
		{
			return *NodePtr;
		}
		return nullptr;
	}
};