// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/Actions/HeartGraphAction.h"
#include "HeartRemoteActionLog.generated.h"

class UHeartGraphNetProxy;

USTRUCT()
struct FHeartRemoteActionLogUndoData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UHeartGraphNetProxy> NetProxy;
};

/**
 * A clientside log of a Heart Action that was RPC'd to the server.
 */
UCLASS(Hidden)
class HEARTNET_API UHeartRemoteActionLog : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	static const FLazyName LogStorage;

protected:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};