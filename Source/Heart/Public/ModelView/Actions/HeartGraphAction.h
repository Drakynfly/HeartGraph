// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphActionBase.h"
#include "HeartGraphAction.generated.h"

class UHeartGraph;
class UHeartGraphNode;
class UHeartGraphPin;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartGraphAction : public UHeartGraphActionBase
{
	GENERATED_BODY()

public:
	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation) override;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual void ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation) {}
};

UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphActionBlueprintBase : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation) override final;
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation) override final;
	virtual void ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation) override final;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	void BP_ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	void BP_ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	void BP_ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation);
};