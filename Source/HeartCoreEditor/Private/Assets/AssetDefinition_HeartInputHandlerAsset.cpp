// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Assets/AssetDefinition_HeartInputHandlerAsset.h"

#include "Input/HeartInputHandlerAssetBase.h"

#include "HeartEditorShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_HeartInputHandlerAsset)

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartInputHandlerAsset"

FText UAssetDefinition_HeartInputHandlerAsset::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDisplayName", "Heart Input Handler Asset");
}

FLinearColor UAssetDefinition_HeartInputHandlerAsset::GetAssetColor() const
{
	return FColor(155, 24, 155);
}

TSoftClassPtr<> UAssetDefinition_HeartInputHandlerAsset::GetAssetClass() const
{
	return UHeartInputHandlerAssetBase::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HeartInputHandlerAsset::GetAssetCategories() const
{
	return Heart::EditorShared::GetAssetCategories();
}

#undef LOCTEXT_NAMESPACE