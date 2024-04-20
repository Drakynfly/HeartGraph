// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodContainer.h"
#include "Components/SlateWrapperTypes.h"
#include "Input/HeartActionBase.h"
#include "HeartGraphSlateAction.generated.h"

class UHeartSlatePtr;
class UHeartSlateGraph;
class UHeartSlateNode;
class UHeartSlatePin;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEARTCANVAS_API UHeartGraphSlateAction : public UHeartActionBase
{
	GENERATED_BODY()

protected:
	//~ UHeartActionBase
	virtual FText GetDescription(const UObject* Target) const override final;
	virtual bool CanExecute(const UObject* Target) const override final;
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) const override final;
	//~ UHeartActionBase

	virtual FText GetDescription(const UHeartSlatePtr* SlatePtr) const;
	virtual bool CanExecuteOnSlatePtr(const UHeartSlatePtr* SlatePtr) const;
	virtual FHeartEvent ExecuteOnSlatePtr(UHeartSlatePtr* SlatePtr, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const;

	virtual FReply ExecuteOnGraph(UHeartSlateGraph* SlateGraph, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }

	virtual FReply ExecuteOnNode(UHeartSlateNode* SlateNode, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }

	virtual FReply ExecuteOnPin(UHeartSlatePin* SlatePin, const FHeartInputActivation& Activation,
		UObject* ContextObject, FBloodContainer& UndoData) const { return FReply::Unhandled(); }
};


UCLASS(Abstract, Blueprintable, MinimalAPI)
class UHeartGraphSlateAction_BlueprintBase final : public UHeartGraphSlateAction
{
	GENERATED_BODY()

protected:
	//~ UHeartActionBase
	virtual bool CanUndo(const UObject* Target) const override;
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
	//~ UHeartActionBase

	//~ UHeartGraphSlateAction
	virtual FText GetDescription(const UHeartSlatePtr* SlatePtr) const override;
	virtual bool CanExecuteOnSlatePtr(const UHeartSlatePtr* SlatePtr) const override;
	virtual FReply ExecuteOnGraph(UHeartSlateGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FReply ExecuteOnNode(UHeartSlateNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual FReply ExecuteOnPin(UHeartSlatePin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	//~ UHeartGraphSlateAction

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Description"))
	FText BP_GetDescription(const UHeartSlatePtr* SlatePtr) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Can Execute on Widget"))
	bool BP_CanExecuteOnWidget(const UHeartSlatePtr* SlatePtr) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	FEventReply BP_ExecuteOnGraph(UHeartSlateGraph* SlateGraph, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	FEventReply BP_ExecuteOnNode(UHeartSlateNode* SlateNode, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	FEventReply BP_ExecuteOnPin(UHeartSlatePin* SlatePin, const FHeartInputActivation& Activation, UObject* ContextObject, UPARAM(ref) FBloodContainer& UndoData) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Undo"))
	bool BP_Undo(UHeartSlatePtr* Target, const FBloodContainer& UndoData) const;
};