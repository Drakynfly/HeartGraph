// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "HeartPinAction_Disconnect.generated.h"

class IHeartGraphPinInterface;

/**
 *
 */
UCLASS()
class HEART_API UHeartPinAction_Disconnect : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(const UObject* Object) const override;
	virtual void ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>&, const FHeartInputActivation& Activation, UObject* ContextObject) override;
};

/**
 *
 */
UCLASS()
class HEART_API UHeartNodeAction_Disconnect : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(const UObject* Object) const override;
	virtual void ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override;
};