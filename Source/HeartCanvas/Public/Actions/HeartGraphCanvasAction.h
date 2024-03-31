// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/SlateWrapperTypes.h"
#include "Input/HeartActionBase.h"
#include "HeartGraphCanvasAction.generated.h"

class UWidget;
class UHeartGraphWidgetBase;
class UHeartGraphCanvas;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEARTCANVAS_API UHeartGraphCanvasAction : public UHeartActionBase
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override final;
	virtual bool Execute(const Heart::Action::FArguments& Arguments) override final;

	virtual FReply ExecuteOnWidget(UHeartGraphWidgetBase* Widget, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual FText GetDescription(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual bool CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject) { return false; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual FEventReply ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject) { return false; }

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual FEventReply ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject) { return false; }
};


UCLASS(Abstract, Blueprintable)
class HEARTCANVAS_API UHeartGraphCanvasAction_BlueprintBase final : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UHeartGraphWidgetBase* Widget) const override;
	virtual bool CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const override;
	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual FEventReply ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual FEventReply ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual bool CanUndo(UObject* Target) const override;
	virtual bool Undo(UObject* Target) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute on Widget"))
	bool BP_CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FEventReply BP_ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FEventReply BP_ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FEventReply BP_ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	bool BP_Undo(UHeartGraphWidgetBase* Target);
};