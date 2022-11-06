// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/Actions/HeartGraphActionBase.h"
#include "HeartGraphCanvasAction.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartGraphCanvasAction : public UHeartGraphActionBase
{
	GENERATED_BODY()

public:
	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation) override;

	UFUNCTION(BlueprintCallable, Category = "HeartGraphAction")
	virtual void ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation) {}

	UFUNCTION(BlueprintCallable, Category = "HeartGraphAction")
	virtual void ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation) {}

	UFUNCTION(BlueprintCallable, Category = "HeartPinAction")
	virtual void ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation) {}
};

UCLASS(Abstract, Blueprintable)
class HEART_API UHeartGraphCanvasActionBlueprintBase : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation) override final;
	virtual void ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation) override final;
	virtual void ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation) override final;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	void BP_ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	void BP_ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	void BP_ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation);
};