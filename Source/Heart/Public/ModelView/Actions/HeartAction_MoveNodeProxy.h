// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "Model/HeartGuids.h"
#include "HeartAction_MoveNodeProxy.generated.h"

USTRUCT()
struct FHeartMoveNodeProxyLocationPair
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D Original = FVector2D::ZeroVector;

	UPROPERTY()
	FVector2D New = FVector2D::ZeroVector;
};

USTRUCT()
struct FHeartMoveNodeProxyUndoData
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FHeartNodeGuid, FHeartMoveNodeProxyLocationPair> Locations;
};

/**
 *
 */
UCLASS(Hidden)
class HEART_API UHeartAction_MoveNodeProxy final : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	static const FLazyName LocationStorage;

protected:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};