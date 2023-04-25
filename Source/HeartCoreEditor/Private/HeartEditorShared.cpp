// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorShared.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_HeartGraph"

TConstArrayView<FAssetCategoryPath> Heart::EditorShared::GetAssetCategories()
{
	static const TArray<FAssetCategoryPath> CategoryPaths = { LOCTEXT("AssetCategory", "Heart") };
	return CategoryPaths;
}

#undef LOCTEXT_NAMESPACE