// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartEvent.h"
#include "HeartInputHandlerAssetBase.h"
#include "HeartInputBindingBase.generated.h"

struct FHeartInputActivation;

/**
 *
 */
UCLASS(Const, Abstract, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartInputBindingBase : public UHeartInputHandlerAssetBase
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

	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
		PURE_VIRTUAL(UHeartInputBindingBase::OnTriggered, return FHeartEvent::Invalid; )
};

/**
 * Base class for deferred handlers
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartInputBinding_Deferred : public UHeartInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
};