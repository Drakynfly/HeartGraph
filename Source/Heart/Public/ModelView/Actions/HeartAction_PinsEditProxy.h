// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "Model/HeartPinConnectionEdit.h"
#include "HeartAction_PinsEditProxy.generated.h"

USTRUCT()
struct FHeartPinsEditProxyUndoData
{
	GENERATED_BODY()

	TMap<FHeartNodeGuid, Heart::API::FPinEdit::FMemento> Original;
	TMap<FHeartNodeGuid, Heart::API::FPinEdit::FMemento> New;

	bool Serialize(FArchive& Ar)
	{
		return !(Ar << Original << New).IsError();
	}
};

template<>
struct TStructOpsTypeTraits<FHeartPinsEditProxyUndoData> : public TStructOpsTypeTraitsBase2<FHeartPinsEditProxyUndoData>
{
	enum
	{
		WithSerializer = true,
	};
};

/**
 *
 */
UCLASS(Hidden)
class HEART_API UHeartAction_PinsEditProxy : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	static const FLazyName MementosStorage;

protected:
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};