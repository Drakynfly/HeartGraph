// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandler_Immediate.h"
#include "Templates/SubclassOf.h"
#include "HeartInputHandler_Action.generated.h"

class UHeartActionBase;

UCLASS(meta = (DisplayName = "Graph Action"))
class HEARTCORE_API UHeartInputHandler_Action : public UHeartInputHandler_Immediate
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