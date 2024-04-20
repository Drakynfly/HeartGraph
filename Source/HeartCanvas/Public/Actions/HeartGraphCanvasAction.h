// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodContainer.h"
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

protected:
	//~ UHeartActionBase
	virtual FText GetDescription(const UObject* Target) const override final;
	virtual bool CanExecute(const UObject* Target) const override final;
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) const override final;
	//~ UHeartActionBase

	virtual FText GetDescription(const UHeartGraphWidgetBase* Widget) const;
	virtual bool CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const;
	virtual FHeartEvent ExecuteOnWidget(UHeartGraphWidgetBase* Widget, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const;

	virtual FReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }

	virtual FReply ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }

	virtual FReply ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }
};


UCLASS(Abstract, Blueprintable, MinimalAPI)
class UHeartGraphCanvasAction_BlueprintBase final : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

protected:
	//~ UHeartActionBase
	virtual bool CanUndo(const UObject* Target) const override;
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
	//~ UHeartActionBase

	//~ UHeartGraphCanvasAction
	virtual FText GetDescription(const UHeartGraphWidgetBase* Widget) const override;
	virtual bool CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const override;
	virtual FReply ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FReply ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FReply ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	//~ UHeartGraphCanvasAction

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute on Widget"))
	bool BP_CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FEventReply BP_ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FEventReply BP_ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FEventReply BP_ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	bool BP_Undo(UHeartGraphWidgetBase* Target, const FBloodContainer& UndoData) const;
};