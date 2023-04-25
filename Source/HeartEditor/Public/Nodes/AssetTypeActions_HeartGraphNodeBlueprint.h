// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

// @todo this will eventually be replaced by a UAssetDefinition_Blueprint, but that isnt properly exposed yet
class HEARTEDITOR_API FAssetTypeActions_HeartGraphNodeBlueprint final : public FAssetTypeActions_Blueprint
{
public:
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(155, 24, 84); }

	virtual UClass* GetSupportedClass() const override;

protected:
	// FAssetTypeActions_Blueprint
	virtual bool CanCreateNewDerivedBlueprint() const override { return false; }
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
	// --
};
