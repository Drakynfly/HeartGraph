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
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject);
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
	virtual bool Undo(UObject* Target) override;
	//~ UHeartActionBase

protected:
	//~ UHeartGraphAction
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	//~ UHeartGraphAction

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute On Object"))
	bool BP_CanExecuteOnObject(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FHeartEvent BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FHeartEvent BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FHeartEvent BP_ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	void BP_Undo(const UObject* Target);
};