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

protected:
	//~ UHeartActionBase
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) const override final;
	//~ UHeartActionBase

	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const;
};


UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphAction_BlueprintBase final : public UHeartGraphAction
{
	GENERATED_BODY()

protected:
	//~ UHeartActionBase
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override;
	virtual bool CanUndo(const UObject* Target) const override;
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
	//~ UHeartActionBase

	//~ UHeartGraphAction
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	//~ UHeartGraphAction


	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute on Object"))
	bool BP_CanExecuteOnObject(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FHeartEvent BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FHeartEvent BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FHeartEvent BP_ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	void BP_Undo(const UObject* Target, const FBloodContainer& UndoData) const;
};