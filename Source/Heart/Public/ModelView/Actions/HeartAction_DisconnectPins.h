// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "Model/HeartPinConnectionEdit.h"
#include "HeartAction_DisconnectPins.generated.h"

class IHeartGraphPinInterface;

/**
 *
 */
UCLASS()
class HEART_API UHeartAction_DisconnectPins : public UHeartGraphAction
{
	GENERATED_BODY()

protected:
	virtual bool CanExecute(const UObject* Object) const override;
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>&, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual bool CanUndo(UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target) override;

private:
	// Undo data
	TWeakObjectPtr<UHeartGraphNode> TargetNode;
	TMap<FHeartNodeGuid, Heart::Connections::FEdit::FMemento> Mementos;
};