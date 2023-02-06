// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class HEARTEDITOR_API FAssetTypeActions_HeartWidgetInputHandlerAsset final : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(155, 24, 155); }

	virtual UClass* GetSupportedClass() const override;
};
