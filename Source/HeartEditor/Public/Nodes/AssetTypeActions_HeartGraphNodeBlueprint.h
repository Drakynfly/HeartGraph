// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Script/AssetDefinition_Blueprint.h"
#include "AssetTypeActions_HeartGraphNodeBlueprint.generated.h"

UCLASS()
class HEARTEDITOR_API UAssetDefinition_HeartGraphNodeBlueprint : public UAssetDefinition_Blueprint
{
	GENERATED_BODY()

public:
	// UAssetDefinition Begin
	virtual FText GetAssetDisplayName() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	// UAssetDefinition End
};