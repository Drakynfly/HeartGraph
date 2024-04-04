// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "Model/HeartPinConnectionEdit.h"
#include "HeartAction_DisconnectPins.generated.h"

class IHeartGraphPinInterface;

USTRUCT()
struct FHeartDisconnectPinsUndoData
{
	GENERATED_BODY()

	TSoftObjectPtr<UHeartGraphNode> TargetNode;
	TMap<FHeartNodeGuid, Heart::Connections::FEdit::FMemento> Mementos;

	bool Serialize(FArchive& Ar);
};

/**
 *
 */
UCLASS()
class HEART_API UHeartAction_DisconnectPins : public UHeartGraphAction
{
	GENERATED_BODY()

protected:
	virtual bool CanExecute(const UObject* Object) const override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>&, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual bool CanUndo(UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) override;
};