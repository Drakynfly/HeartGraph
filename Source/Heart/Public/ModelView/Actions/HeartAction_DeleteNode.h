// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "Model/HeartPinConnectionEdit.h"
#include "HeartAction_DeleteNode.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartAction_DeleteNode : public UHeartGraphAction
{
	GENERATED_BODY()

protected:
	virtual bool CanExecute(const UObject* Object) const override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual bool CanUndo(UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) override;

private:
	// Undo data
	UPROPERTY()
	TObjectPtr<UHeartGraphNode> DeletedNode;

	TMap<FHeartNodeGuid, Heart::Connections::FEdit::FMemento> Mementos;
};