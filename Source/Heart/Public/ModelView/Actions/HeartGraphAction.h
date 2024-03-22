// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphActionBase.h"
#include "HeartGraphAction.generated.h"

class UHeartGraph;
class UHeartGraphNode;
class IHeartGraphPinInterface;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartGraphAction : public UHeartGraphActionBase
{
	GENERATED_BODY()

public:
	virtual bool Execute(const Heart::Action::FArguments& Arguments) override final;

protected:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) {}
};


UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphAction_BlueprintBase final : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override;
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual bool CanUndo(UObject* Target) const override;
	virtual bool Undo(UObject* Target) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute On Object"))
	bool BP_CanExecuteOnObject(const UObject* Target) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	void BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	void BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	void BP_ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	void BP_Undo(const UObject* Target);
};