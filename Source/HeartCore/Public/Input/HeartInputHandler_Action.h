// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandlerBase.h"
#include "HeartInputHandler_Action.generated.h"

class UHeartActionBase;

UCLASS(meta = (DisplayName = "Graph Action"))
class HEARTCORE_API UHeartInputHandler_Action : public UHeartInputHandlerBase
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* TestTarget) const override;
	virtual bool PassCondition(const UObject* TestTarget) const override;
	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const override;

	void SetAction(const TSubclassOf<UHeartActionBase> Class)
	{
		ActionClass = Class;
	}

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartActionBase> ActionClass;
};