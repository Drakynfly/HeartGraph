// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputBindingBase.h"
#include "HeartSlateInputBinding.generated.h"

struct FHeartInputActivation;

/**
 * Base class for any slate input handler
 */
UCLASS(Abstract)
class UHeartSlateInputBindingBase : public UHeartInputBindingBase
{
	GENERATED_BODY()

public:
	HEARTCANVAS_API virtual FText GetDescription(const TSharedRef<SWidget>& TestWidget) const;
	HEARTCANVAS_API virtual bool PassCondition(const TSharedRef<SWidget>& TestWidget) const;
};

/**
 * Base class for simple slate handlers
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartSlateInputHandlerBase : public UHeartSlateInputBindingBase
{
	GENERATED_BODY()

public:
	// Bind triggers to OnTriggered function. Children should override that, instead of these.
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual FReply OnTriggered(TSharedRef<SWidget>& Widget, const FHeartInputActivation& Trip) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};