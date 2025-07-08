// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/AssetTypeActions_HeartGraphNodeBlueprint.h"
#include "Model/HeartGraphNodeBlueprint.h"
#include "HeartEditorShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetTypeActions_HeartGraphNodeBlueprint)

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartGraphNodeBlueprint"

FText UAssetDefinition_HeartGraphNodeBlueprint::GetAssetDisplayName() const
{
	return LOCTEXT("DisplayName", "Heart Graph Node Blueprint");
}

FText UAssetDefinition_HeartGraphNodeBlueprint::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("Description", "New Blueprint for a Heart Graph Node class");
}

FLinearColor UAssetDefinition_HeartGraphNodeBlueprint::GetAssetColor() const
{
	return Heart::EditorShared::HeartColorDark;
}

TSoftClassPtr<UObject> UAssetDefinition_HeartGraphNodeBlueprint::GetAssetClass() const
{
	return UHeartGraphNodeBlueprint::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HeartGraphNodeBlueprint::GetAssetCategories() const
{
	return Heart::EditorShared::GetAssetCategories();
}

#undef LOCTEXT_NAMESPACE