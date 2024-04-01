// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartEvent.h"
#include "Input/HeartInputBindingBase.h"
#include "HeartInputHandlerBase.generated.h"

struct FHeartInputActivation;

/**
 * Base class for simple handlers
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartInputHandlerBase : public UHeartInputBindingBase
{
	GENERATED_BODY()

public:
	// Bind triggers to OnTriggered function. Children should override that, instead of these.
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Trip) const
		PURE_VIRTUAL(UHeartInputHandlerBase::OnTriggered, return FHeartEvent::Invalid; )

protected:
	// Does this handler block input from bubbling (a capture), or allow other handlers to also respond to it.
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};