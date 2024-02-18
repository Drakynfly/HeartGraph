// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Assets/AssetDefinition_HeartWidgetInputHandlerAsset.h"

#include "UI/HeartWidgetInputHandlerAsset.h"
#include "HeartEditorShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_HeartWidgetInputHandlerAsset)

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartWidgetInputHandlerAsset"

FText UAssetDefinition_HeartWidgetInputHandlerAsset::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDisplayName", "Heart Widget Input Handler Asset");
}

FLinearColor UAssetDefinition_HeartWidgetInputHandlerAsset::GetAssetColor() const
{
	return FColor(155, 24, 155);
}

TSoftClassPtr<> UAssetDefinition_HeartWidgetInputHandlerAsset::GetAssetClass() const
{
	return UHeartWidgetInputHandlerAsset::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HeartWidgetInputHandlerAsset::GetAssetCategories() const
{
	return Heart::EditorShared::GetAssetCategories();
}

#undef LOCTEXT_NAMESPACE