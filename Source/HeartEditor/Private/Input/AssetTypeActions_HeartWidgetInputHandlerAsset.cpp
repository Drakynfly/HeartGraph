// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/AssetTypeActions_HeartWidgetInputHandlerAsset.h"

#include "HeartEditorModule.h"
#include "UI/HeartWidgetInputHandlerAsset.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_HeartWidgetInputHandlerAsset"

FText FAssetTypeActions_HeartWidgetInputHandlerAsset::GetName() const
{
	return LOCTEXT("AssetTypeActions_HeartWidgetInputHandlerAsset", "Heart Widget Input Handler Asset");
}

uint32 FAssetTypeActions_HeartWidgetInputHandlerAsset::GetCategories()
{
	return FHeartEditorModule::HeartAssetCategory;
}

UClass* FAssetTypeActions_HeartWidgetInputHandlerAsset::GetSupportedClass() const
{
	return UHeartWidgetInputHandlerAsset::StaticClass();
}

#undef LOCTEXT_NAMESPACE
