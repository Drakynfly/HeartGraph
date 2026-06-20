// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphComponentBase.h"
#include "HeartGraphNodeComponent.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class HEART_API UHeartGraphNodeComponent : public UHeartGraphComponentBase
{
	GENERATED_BODY()
};


USTRUCT()
struct FHeartGraphNodeComponentMap
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "NodeComponents")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNodeComponent>> Components;

	bool IsEmpty() const;

	TObjectPtr<UHeartGraphNodeComponent> Find(const FHeartNodeGuid& Node) const;
};