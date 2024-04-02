// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "HeartHistoryActions.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartUndoAction : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
};

/**
 *
 */
UCLASS()
class HEART_API UHeartRedoAction : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
};