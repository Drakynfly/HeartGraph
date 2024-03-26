// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

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
	virtual FReply OnTriggered(UObject* Target, const FHeartInputActivation& Trip) const
		PURE_VIRTUAL(UHeartInputHandlerBase::OnTriggered, return FReply::Unhandled(); )

protected:
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};