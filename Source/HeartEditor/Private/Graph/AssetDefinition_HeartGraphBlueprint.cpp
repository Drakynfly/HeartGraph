// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetDefinition_HeartGraphBlueprint.h"
#include "Model/HeartGraphBlueprint.h"
#include "HeartEditorShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_HeartGraphBlueprint)

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartGraphBlueprint"

FText UAssetDefinition_HeartGraphBlueprint::GetAssetDisplayName() const
{
	return LOCTEXT("DisplayName", "Heart Graph Blueprint");
}

FText UAssetDefinition_HeartGraphBlueprint::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("Description", "New Blueprint for a Heart Graph class");
}

FLinearColor UAssetDefinition_HeartGraphBlueprint::GetAssetColor() const
{
	return Heart::EditorShared::HeartColorDark;
}

TSoftClassPtr<UObject> UAssetDefinition_HeartGraphBlueprint::GetAssetClass() const
{
	return UHeartGraphBlueprint::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HeartGraphBlueprint::GetAssetCategories() const
{
	return Heart::EditorShared::GetAssetCategories();
}

#undef LOCTEXT_NAMESPACE