// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartEditorShared.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "Model/HeartGraphNodeBlueprint.h"
#include "Script/AssetDefinition_Blueprint.h"

//#include "AssetTypeActions_HeartGraphNodeBlueprint.generated.h"

// @todo this will eventually be replaced by a UAssetDefinition_Blueprint, but that isn't properly exposed yet
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

/**
 *
 */
/*
UCLASS()
class HEARTEDITOR_API UAssetDefinition_HeartGraphNodeBlueprint : public UAssetDefinition_Blueprint
{
	GENERATED_BODY()

public:
	// UAssetDefinition Begin
	virtual FText GetAssetDisplayName() const override
	{
		return NSLOCTEXT("AssetDefinitions", "FaerieItemAsset", "Faerie Item Asset");
	}
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override
	{
		return NSLOCTEXT("AssetDefinitions", "FaerieItemAsset_Desc", "An asset to create a simple faerie item definition.");
	}
	virtual FLinearColor GetAssetColor() const override { return FLinearColor(FColor(127, 0, 155)); } // Nice purple color
	virtual TSoftClassPtr<UObject> GetAssetClass() const override { return UHeartGraphNodeBlueprint::StaticClass(); }
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
	{
		return Heart::EditorShared::GetAssetCategories();
	}
	// UAssetDefinition End
};
*/