// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartActionBase.h"
#include "HeartGraphAction.generated.h"

class UHeartGraph;
class UHeartGraphNode;
class IHeartGraphPinInterface;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartGraphAction : public UHeartActionBase
{
	GENERATED_BODY()

public:
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) override final;

protected:
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData);
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData);
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData);
};


UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphAction_BlueprintBase final : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	//~ UHeartActionBase
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override;
	virtual bool CanUndo(UObject* Target) const override;
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) override;
	//~ UHeartActionBase

protected:
	//~ UHeartGraphAction
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	//~ UHeartGraphAction

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute On Object"))
	bool BP_CanExecuteOnObject(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FHeartEvent BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FHeartEvent BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FHeartEvent BP_ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	void BP_Undo(const UObject* Target, const FBloodContainer& UndoData);
};