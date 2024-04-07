// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartEvent.h"
#include "HeartInputHandlerAssetBase.generated.h"

struct FHeartInputActivation;
class UHeartInputLinkerBase;

/**
 * Base class for all Input Handler assets.
 */
UCLASS(Abstract, Const)
class HEARTCORE_API UHeartInputHandlerAssetBase : public UObject
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* TestTarget) const
	{
		return FText::GetEmpty();
	}

	virtual bool PassCondition(const UObject* TestTarget) const
	{
		return true;
	}

	virtual Heart::Input::EExecutionOrder GetExecutionOrder() const
		PURE_VIRTUAL(UHeartInputHandlerAssetBase::GetExecutionOrder, return Heart::Input::None; )

	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
		PURE_VIRTUAL(UHeartInputHandlerAssetBase::OnTriggered, return FHeartEvent::Invalid; )
};