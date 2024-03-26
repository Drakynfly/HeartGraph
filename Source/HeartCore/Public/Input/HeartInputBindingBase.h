// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandlerAssetBase.h"
#include "HeartInputBindingBase.generated.h"

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
};