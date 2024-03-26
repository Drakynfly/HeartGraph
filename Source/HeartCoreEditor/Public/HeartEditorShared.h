// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Misc/AssetCategoryPath.h"

namespace Heart::EditorShared
{
	static FLinearColor HeartColor(FColor(255, 24, 44));

	HEARTCOREEDITOR_API TConstArrayView<FAssetCategoryPath> GetAssetCategories();

	HEARTCOREEDITOR_API bool CheckOutFile(const FString& FileName, bool ShowNotification);
}