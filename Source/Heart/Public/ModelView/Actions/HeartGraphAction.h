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
	virtual bool Execute(UObject* Object, const Heart::Action::FArguments& Arguments) override final;

protected:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) {}
};

// @todo blueprintbase should be hyphenated
UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphActionBlueprintBase final : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* Object) const override;
	virtual bool CanExecute(const UObject* Object) const override;
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UObject* Object) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute On Object"))
	bool BP_CanExecuteOnObject(const UObject* Object) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	void BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	void BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	void BP_ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject) const;
};